#include <sys/stat.h>
#include "snapshot.h"

int init_snapshot(SNAPSHOT* ss) {
	if (ss == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	ss->restored = 0;		// by default it's generated
	ss->tf_path = NULL;
	ss->ht = malloc(sizeof(SSHASH_HEADER) * HASH_MAX);

	// assuming NULL == 0
	memset(ss->ht, 0, sizeof(SSHASH_HEADER) * HASH_MAX);

	return 1;
}

// does not free SNAPSHOT itself
int destroy_snapshot(SNAPSHOT* ss) {
	hash_t i;
	SSENTRY* ssentry = NULL;
	void* mustdie = NULL;

	if (ss == NULL || ss->ht == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	for (i = 0; i < HASH_MAX; i++) {
		ssentry = ss->ht[i].first;

		if (ss->restored) {
			if (ssentry != NULL) {
				free(ssentry);		// all entries at once
			}
		} else {
			while (ssentry != NULL) {
				mustdie = ssentry;
				ssentry = ssentry->next;
				free(mustdie);
			}
		}
	}

	free(ss->ht);
	ss->ht = NULL;

	if (ss->tf_path != NULL) {
		free(ss->tf_path);
	}

	return 1;
}

int take_snapshot(char* path, SNAPSHOT* ss) {
	int len = 0;

	if (ss == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	init_snapshot(ss);

	len = strlen(path);
	// trim slash at the end if necessary
	if (len > 1 && (path[len] == '/' || path[len] == '\\')) {
		len--;
		path[len] = 0;
	}

	if (!process_dir(path, len, ss)) {
		destroy_snapshot(ss);
		return 0;
	}

	ss->tf_path = strdup(path);

	return 1;
}

SSENTRY* search(char* path, SNAPSHOT* ss) {
	hash_t hash;
	SSENTRY* ssentry = NULL;
	size_t pathlen;

	if (ss == NULL || ss->ht == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return NULL;
	}

	pathlen = strlen(path);
	hash = uint16_hash(path, pathlen);

	ssentry = ss->ht[hash].first;
	while (ssentry != NULL) {
		if (strncmp(path, SSENTRY_PATH(ssentry), pathlen + 1) == 0) {
			break;
		}
		ssentry = ssentry->next;
	}

	return ssentry;
}

int process_dir(char* path, size_t skip, SNAPSHOT* ss) {
	DIR* dir = NULL;
	struct dirent* entry = NULL;
	int res = 1;

	PLOG("Processing dir: %s\n", path);
	dir = opendir(path);
	if (dir == NULL) {
		PERR("Cannot open dir %s: %s\n", path, strerror(errno));
		return 0;
	}

	while (1) {
		// Windows port dirent.h does not have readdir_r().
		// BTW we don't need it here.
		entry = readdir(dir);
		if (entry == NULL) {
			// an error or the end of the directory
			break;
		}

		if ((strncmp(entry->d_name, ".", 2) == 0) || 
			(strncmp(entry->d_name, "..", 3) == 0)) {
			continue;
		}

		if (!process_entry(path, skip, entry->d_name, ss)) {
			res = 0;
			break;
		}
	}

	if (closedir(dir) < 0) {
		PERR("Cannot close dir %s: %s\n", path, strerror(errno));
		return 0;
	}

	return res;
}

int process_entry(char* path, size_t skip, char* name, SNAPSHOT* ss) {
	struct stat entry_info;
	SSENTRY* ssentry = NULL;
	size_t pathmem = 0;
	char* path_full = NULL;		// path with a root dir of this processing
	size_t pl = 0;
	size_t nl = 0;

	pl = strlen(path);
	nl = strlen(name);

	// allocating memory for SSENTRY + path, pathmem will be aligned to WORD_SIZE
	// in order to get properly aligned memory after load_snapshot()
	pathmem = (pl - skip + nl + 1 + WORD_SIZE) & ~(WORD_SIZE - 1);
	ssentry = malloc(sizeof(SSENTRY) + pathmem);

	if (ssentry == NULL) {
		PERR("Cannot allocate memory for an entry: %s\n", strerror(errno));
		return 0;
	}

	path_full = malloc(pathmem + skip + 1);
	strncpy(path_full, path, pl + 1);
	strncat(path_full, "/", 2);
	strncat(path_full, name, nl + 1);

	ssentry->status = 0;
	ssentry->custom = 0;
	ssentry->pathmem = pathmem;
	strncpy(SSENTRY_PATH(ssentry), path_full + skip + 1, pathmem);

	if (stat(path_full, &entry_info)) {
		PERR("Cannot get info about %s: %s\n", path_full, strerror(errno));
		free(ssentry);
		free(path_full);
		return 0;
	}

	ssentry->content.size = entry_info.st_size;
	ssentry->content.mtime = entry_info.st_mtime;

	if (S_ISDIR(entry_info.st_mode)) {
		ssentry->status |= SSENTRY_STATUS_DIR;
		process_dir(path_full, skip, ss);
	} else if (S_ISREG(entry_info.st_mode)) {
		ssentry->status &= ~SSENTRY_STATUS_DIR;
	} else {
		PLOG("Skipping irregular file: %s\n", path_full);
		free(ssentry);
		free(path_full);
		return 1;		// it is a successful operation
	}

	free(path_full);
	return add_to_snapshot(ssentry, ss);
}

int add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss) {
	hash_t hash;

	if (ss->restored) {
		PERR("Adding entries to a restored snapshot is denied.");
		return 0;
	}

	hash = uint16_hash(SSENTRY_PATH(ssentry), strlen(SSENTRY_PATH(ssentry)));

	// push to the beginning of the list
	ssentry->next = ss->ht[hash].first;
	ss->ht[hash].first = ssentry;
	ss->ht[hash].size += sizeof(SSENTRY) + ssentry->pathmem;

	return 1;
}

int find_changes(SNAPSHOT* ss0, SNAPSHOT* ss1) {
	hash_t i;
	SSENTRY* ssentry = NULL;
	SSENTRY* found = NULL;
	char* path = NULL;
	int differs = 0;

	if (ss0 == NULL || ss0->ht == NULL || ss1 == NULL || ss1->ht == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;	// TODO what we need return?
	}

	for (i = 0; i < HASH_MAX; i++) {
		ssentry = ss1->ht[i].first;

		while (ssentry != NULL) {
			path = SSENTRY_PATH(ssentry);
			found = search(path, ss0);

			if (found == NULL) {
				if (differs == 0) {
					differs = 1;
				}
				printf("CHANGE_2: %s\n", path);
			} else {
				found->custom = 1;		// not to be checked at the loop over ss0
				if (found->status != ssentry->status
						|| found->content.mtime != ssentry->content.mtime
						|| found->content.size != ssentry->content.size) {
					if (differs == 0) {
						differs = 1;
					}
					printf("CHANGE_1: %s\n", path);
				}
			}

			ssentry = ssentry->next;
		}
	}

	for (i = 0; i < HASH_MAX; i++) {
		ssentry = ss0->ht[i].first;

		while (ssentry != NULL) {
			if (ssentry->custom == 0) {
				path = SSENTRY_PATH(ssentry);
				found = search(path, ss1);

				if (found == NULL) {
					if (differs == 0) {
						differs = 1;
					}
					printf("CHANGE_3: %s\n", path);
				}
			} else {
				ssentry->custom = 0;	// resetting to a default value
			}

			ssentry = ssentry->next;
		}
	}

	return differs;
}

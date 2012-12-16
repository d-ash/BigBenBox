#include <sys/stat.h>
#include "snapshot.h"

int init_snapshot(SNAPSHOT* ss) {
	if (ss == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	ss->restored = 0;		// by default it's generated
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

	return 1;
}

int take_snapshot(char* path, SNAPSHOT* ss) {
	if (ss == NULL) {
		PERR("NULL value in %s()\n", __FUNCTION__);
		return 0;
	}

	init_snapshot(ss);

	if (!process_dir(path, ss)) {
		destroy_snapshot(ss);
		return 0;
	}

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

int process_dir(char* path, SNAPSHOT* ss) {
	DIR* dir = NULL;
	struct dirent entry;
	struct dirent* entry_ptr = NULL;
	int res = 1;

	PLOG("Processing dir: %s\n", path);
	dir = opendir(path);
	if (dir == NULL) {
		PERR("Cannot open dir %s: %s\n", path, strerror(errno));
		return 0;
	}

	while (1) {
		readdir_r(dir, &entry, &entry_ptr);
		if (entry_ptr == NULL) {
			break;
		}

		if ((strncmp(entry.d_name, ".", 2) == 0) || 
			(strncmp(entry.d_name, "..", 3) == 0)) {
			continue;
		}

		if (!process_entry(path, entry.d_name, ss)) {
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

int process_entry(char* path, char* name, SNAPSHOT* ss) {
	struct stat entry_info;
	SSENTRY* ssentry = NULL;
	size_t pathmem = 0;
	char* path_ptr = NULL;
	size_t pl = 0;
	size_t nl = 0;

	pl = strlen(path);
	nl = strlen(name);

	// allocating memory for SSENTRY + path
	pathmem = pl + nl + 2;
	ssentry = malloc(sizeof(SSENTRY) + pathmem);

	if (ssentry == NULL) {
		PERR("Cannot allocate memory for an entry: %s\n", strerror(errno));
		return 0;
	}

	ssentry->status = 0;
	ssentry->pathmem = pathmem;

	path_ptr = SSENTRY_PATH(ssentry);
	strncpy(path_ptr, path, pl + 1);
	strncat(path_ptr, "/", 2);
	strncat(path_ptr, name, nl + 1);

	if (lstat(path_ptr, &entry_info)) {
		PERR("Cannot get info about %s: %s\n", path_ptr, strerror(errno));
		free(ssentry);
		return 0;
	}

	ssentry->content.size = entry_info.st_size;
	ssentry->content.mtime = entry_info.st_mtime;

	if (S_ISDIR(entry_info.st_mode)) {
		ssentry->status |= SSENTRY_STATUS_DIR;
		process_dir(path_ptr, ss);
	} else if (S_ISREG(entry_info.st_mode)) {
		ssentry->status &= ~SSENTRY_STATUS_DIR;
	} else {
		PLOG("Skipping irregular file: %s\n", path_ptr);
		free(ssentry);
		return 1;		// it is a successful operation
	}

	return add_to_snapshot(ssentry, ss);
}

int add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss) {
	hash_t hash;

	if (ss->restored) {
		PERR("Adding entries to a restored snapshot is denied.");
		return 0;
	}

	hash = uint16_hash(SSENTRY_PATH(ssentry), ssentry->pathmem - 1);

	// push to the beginning of the list
	ssentry->next = ss->ht[hash].first;
	ss->ht[hash].first = ssentry;
	ss->ht[hash].size += sizeof(SSENTRY) + ssentry->pathmem;

	return 1;
}

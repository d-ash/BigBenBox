#include "snapshot.h"

// TODO refactor! copy-paste

SSHASHTABLE* find_hashtable(uint16_t hash, SSHASHTABLE* ht) {
	while (ht != NULL) {
		if (ht->hash == hash) {
			break;
		}
		ht = ht->next;
	}

	return ht;
}

void add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss) {
	uint16_t hash;
	SSHASHTABLE* ht = NULL;

	ss->entries_count += 1;
	ss->pathmem_total += ssentry->header.pathmem;

	// put this entry to ss->ht_by_hash
	// using the first two bytes of the filehash
	// it is faster than: uint16_hash();
	hash = (* (uint16_t*) ssentry->header.content.filehash);
	ht = find_hashtable(hash, ss->ht_by_hash);

	if (ht == NULL) {
		// it is the first entry with this hash
		ssentry->next_by_hash = NULL;

		// create a new hashtable for it
		ht = malloc(sizeof(SSHASHTABLE));
		ht->hash = hash;
		ht->entries = ssentry;

		// insert this hashtable at the beginning of the list
		ht->next = ss->ht_by_hash;
		ss->ht_by_hash = ht;
	} else {
		// insert this entry at the beginning of the found hashtable
		ssentry->next_by_hash = ht->entries;
		ht->entries = ssentry;
	}

	// put this entry to ss->ht_by_path
	hash = uint16_hash(SSENTRY_PATH(ssentry), ssentry->header.pathmem - 1);
	ht = find_hashtable(hash, ss->ht_by_path);

	if (ht == NULL) {
		// it is the first entry with this hash
		ssentry->next_by_path = NULL;

		// create a new hashtable for it
		ht = malloc(sizeof(SSHASHTABLE));
		ht->hash = hash;
		ht->entries = ssentry;

		// insert this hashtable at the beginning of the list
		ht->next = ss->ht_by_path;
		ss->ht_by_path = ht;
	} else {
		// insert this entry at the beginning of the found hashtable
		ssentry->next_by_path = ht->entries;
		ht->entries = ssentry;
	}
}

SSENTRY* search_by_path(char* path, SNAPSHOT* ss) {
	uint16_t hash;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;

	hash = uint16_hash(path, strlen(path));
	ht = find_hashtable(hash, ss->ht_by_path);
	if (ht == NULL) {
		return NULL;
	} else {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			if (strncmp(path, SSENTRY_PATH(ssentry), PATH_MAX) == 0) {
				break;
			}
			ssentry = ssentry->next_by_path;
		}
		return ssentry;
	}
}

SSENTRY* search_by_filehash(unsigned char* filehash, SNAPSHOT* ss) {
	uint16_t hash;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;

	hash = (* (uint16_t*) filehash);
	ht = find_hashtable(hash, ss->ht_by_hash);
	if (ht == NULL) {
		return NULL;
	} else {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			if (memcmp(filehash, ssentry->header.content.filehash, FILEHASH_LENGTH) == 0) {
				break;
			}
			ssentry = ssentry->next_by_hash;
		}
		return ssentry;
	}
}

void process_entry(char* path, char* name, SNAPSHOT* ss) {
	int processed = 0;
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
	ssentry->header.status = 0;
	ssentry->header.pathmem = pathmem;

	path_ptr = SSENTRY_PATH(ssentry);
	strncpy(path_ptr, path, pl + 1);
	strncat(path_ptr, "/", 2);
	strncat(path_ptr, name, nl + 1);

	ssentry->header.content.filesize = 0;
	memset(ssentry->header.content.filehash, 0, FILEHASH_LENGTH);

	if (lstat(path_ptr, &entry_info)) {
		printf("Cannot get info about %s: %s\n", path_ptr, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (S_ISDIR(entry_info.st_mode)) {
		ssentry->header.status |= SSENTRY_STATUS_DIR;
		process_dir(path_ptr, ss);
		processed = 1;
	} else if (S_ISREG(entry_info.st_mode)) {
		ssentry->header.status &= ~SSENTRY_STATUS_DIR;
		ssentry->header.content.filesize = entry_info.st_size;
		if (entry_info.st_size > 0) {
			hash_file(path_ptr, entry_info.st_size, ssentry->header.content.filehash);
		}
		processed = 1;
	}

	if (processed) {
		add_to_snapshot(ssentry, ss);
	} else {
		DEBUG_LOG("Skipping irregular file: %s\n", path_ptr);
		free(ssentry);
	}
}

void process_dir(char* path, SNAPSHOT* ss) {
	DIR* dir = NULL;
	struct dirent entry;
	struct dirent* entry_ptr = NULL;

	DEBUG_LOG("Processing dir: %s\n", path);
	dir = opendir(path);
	if (dir == NULL) {
		printf("Cannot open dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		readdir_r(dir, &entry, &entry_ptr);
		if (entry_ptr == NULL) {
			break;
		}

		if ((strncmp(entry.d_name, ".", PATH_MAX) == 0) || 
			(strncmp(entry.d_name, "..", PATH_MAX) == 0)) {
			continue;
		}

		process_entry(path, entry.d_name, ss);
	}

	if (closedir(dir) < 0) {
		printf("Cannot close dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

SNAPSHOT* create_snapshot(char* path) {
	SNAPSHOT* ss;

	ss = malloc(sizeof(SNAPSHOT));
	ss->ht_by_hash = NULL;
	ss->ht_by_path = NULL;
	ss->entries_count = 0;
	ss->pathmem_total = 0;

	process_dir(path, ss);

	return ss;
}

void destroy_snapshot(SNAPSHOT* ss) {
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;
	void* mustdie = NULL;

	ht = ss->ht_by_hash;
	while (ht != NULL) {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			mustdie = ssentry;
			ssentry = ssentry->next_by_hash;
			free(mustdie);
		}
		mustdie = ht;
		ht = ht->next;
		free(mustdie);
	}

	ht = ss->ht_by_path;
	while (ht != NULL) {
		mustdie = ht;
		ht = ht->next;
		free(mustdie);
	}

	free(ss);
}

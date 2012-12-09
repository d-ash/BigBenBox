#include "snapshot.h"

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

	// put this entry to ss->ht_by_hash
	// using the first two bytes of the hash, it is faster than:
	// hash = uint16_hash(ssentry->hash, MD5_DIGEST_LENGTH);
	hash = (* (uint16_t*) ssentry->hash);
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
	hash = uint16_hash(ssentry->path, strlen(ssentry->path));
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
			if (strncmp(path, ssentry->path, PATH_MAX) == 0) {
				break;
			}
			ssentry = ssentry->next_by_path;
		}
		return ssentry;
	}
}

SSENTRY* search_by_hash(unsigned char* longhash, SNAPSHOT* ss) {
	uint16_t hash;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;

	hash = (* (uint16_t*) longhash);
	ht = find_hashtable(hash, ss->ht_by_hash);
	if (ht == NULL) {
		return NULL;
	} else {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			if (memcmp(longhash, ssentry->hash, MD5_DIGEST_LENGTH) == 0) {
				break;
			}
			ssentry = ssentry->next_by_hash;
		}
		return ssentry;
	}
}

void serialize_entry(SSENTRY* ssentry) {
	printf("%s,%s,", ssentry->is_dir ? "1" : "0", ssentry->is_empty ? "1" : "0");
	print_md5_sum(ssentry->hash);
	printf(",%llu,\"%s\"\n", (long long) ssentry->size, ssentry->path);
}

void serialize_snapshot(SNAPSHOT* ss) {
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;

	ht = ss->ht_by_hash;
	while (ht != NULL) {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			serialize_entry(ssentry);
			ssentry = ssentry->next_by_hash;
		}
		ht = ht->next;
	}
}

// Returns 1 if the dir is empty, otherwise 0.
int process_dir(char* path, SNAPSHOT* ss) {
	DIR* dir = NULL;
	int is_empty = 0;

	//printf("Processing dir: %s\n", path);
	dir = opendir(path);
	if (dir == NULL) {
		printf("Cannot open dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	while (1) {
		int retval = 0;
		struct dirent entry;
		struct dirent* entry_ptr = NULL;
		struct stat entry_info;
		char entry_path[PATH_MAX + 1];
		SSENTRY* ssentry = NULL;

		retval = readdir_r(dir, &entry, &entry_ptr);
		if (entry_ptr == NULL) {
			break;
		}
		if ((strncmp(entry.d_name, ".", PATH_MAX) == 0) || 
			(strncmp(entry.d_name, "..", PATH_MAX) == 0)) {
			continue;
		}

		strncpy(entry_path, path, PATH_MAX);
		strncat(entry_path, "/", 1);
		strncat(entry_path, entry.d_name, PATH_MAX - strlen(entry_path));

		if (lstat(entry_path, &entry_info)) {
			printf("Cannot get info about %s: %s\n", entry_path, strerror(errno));
			exit(EXIT_FAILURE);
		}

		ssentry = malloc(sizeof(SSENTRY));
		ssentry->path = strdup(entry_path);
		if (ssentry->path == NULL) {
			printf("Cannot strdup(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		ssentry->size = 0;
		memset(ssentry->hash, 0, MD5_DIGEST_LENGTH);

		if (S_ISDIR(entry_info.st_mode)) {
			ssentry->is_dir = 1;
			ssentry->is_empty = process_dir(entry_path, ss);
			if (is_empty) {
				is_empty = 0;
			}
		} else if (S_ISREG(entry_info.st_mode)) {
			ssentry->is_dir = 0;
			if (entry_info.st_size > 0) {
				ssentry->is_empty = 0;
				ssentry->size = entry_info.st_size;
				hash_file(entry_path, entry_info.st_size, ssentry->hash);
			} else {
				ssentry->is_empty = 1;
			}
			if (is_empty) {
				is_empty = 0;
			}
		} else {
			//printf("Skipping %s\n", entry_path);
		}

		add_to_snapshot(ssentry, ss);
	}
	if (closedir(dir) < 0) {
		printf("Cannot close dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return is_empty;
}

SNAPSHOT* create_snapshot(char* path) {
	SNAPSHOT* ss;

	ss = malloc(sizeof(SNAPSHOT));
	ss->ht_by_hash = NULL;
	ss->ht_by_path = NULL;

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
			free(ssentry->path);
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
}

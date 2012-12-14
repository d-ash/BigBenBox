#include <sys/stat.h>
#include "snapshot.h"

void add_to_snapshot(SSENTRY* ssentry, SNAPSHOT* ss) {
	hash_t hash;

	hash = uint16_hash(SSENTRY_PATH(ssentry), ssentry->header.pathmem - 1);

	ssentry->next = ss->ht[hash];
	ss->ht[hash] = ssentry;
}

SSENTRY* search(char* path, SNAPSHOT* ss) {
	hash_t hash;
	SSENTRY* ssentry = NULL;
	size_t pathlen;

	pathlen = strlen(path);
	hash = uint16_hash(path, pathlen);

	ssentry = ss->ht[hash];
	while (ssentry != NULL) {
		if (strncmp(path, SSENTRY_PATH(ssentry), pathlen + 1) == 0) {
			break;
		}
		ssentry = ssentry->next;
	}

	return ssentry;
}

void process_entry(char* path, char* name, SNAPSHOT* ss) {
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

	if (lstat(path_ptr, &entry_info)) {
		PERR("Cannot get info about %s: %s\n", path_ptr, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssentry->header.content.size = entry_info.st_size;
	ssentry->header.content.mtime = entry_info.st_mtime;

	if (S_ISDIR(entry_info.st_mode)) {
		ssentry->header.status |= SSENTRY_STATUS_DIR;
		process_dir(path_ptr, ss);
	} else if (S_ISREG(entry_info.st_mode)) {
		ssentry->header.status &= ~SSENTRY_STATUS_DIR;
	} else {
		PLOG("Skipping irregular file: %s\n", path_ptr);
		free(ssentry);
		return;
	}

	add_to_snapshot(ssentry, ss);
}

void process_dir(char* path, SNAPSHOT* ss) {
	DIR* dir = NULL;
	struct dirent entry;
	struct dirent* entry_ptr = NULL;

	PLOG("Processing dir: %s\n", path);
	dir = opendir(path);
	if (dir == NULL) {
		PERR("Cannot open dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
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

		process_entry(path, entry.d_name, ss);
	}

	if (closedir(dir) < 0) {
		PERR("Cannot close dir %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

SNAPSHOT* create_snapshot(char* path) {
	SNAPSHOT* ss;

	ss = malloc(sizeof(SNAPSHOT));
	// assuming NULL == 0
	memset(ss->ht, 0, sizeof(void*) * HASH_MAX);

	process_dir(path, ss);

	return ss;
}

void destroy_snapshot(SNAPSHOT* ss) {
	hash_t i;
	SSENTRY* ssentry = NULL;
	void* mustdie = NULL;

	for (i = 0; i < HASH_MAX; i++) {
		ssentry = ss->ht[i];
		while (ssentry != NULL) {
			mustdie = ssentry;
			ssentry = ssentry->next;
			free(mustdie);
		}
	}

	free(ss);
}

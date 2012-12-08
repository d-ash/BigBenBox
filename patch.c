#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/md5.h>

#define SS_ALLOC_PAGE 1


// TODO change storage (two binary trees by PATH and by HASH)

typedef struct s_ssentry {
	char path[PATH_MAX + 1];
	int is_dir;
	int is_empty;
	unsigned char hash[MD5_DIGEST_LENGTH];
	void* next;
} SSENTRY;

typedef struct s_snapshot {
	SSENTRY* first;
	SSENTRY* last;
} SNAPSHOT;

// Print the MD5 sum as hex-digits.
void print_md5_sum(unsigned char* md) {
    int i;

    for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		printf("%02x", md[i]);
    }
}

void output_ssentries(SSENTRY* ssentry) {
	printf("%s\t%s\t", ssentry->is_dir ? "DIR" : "", ssentry->is_empty ? "EMPTY" : "");
	if (ssentry->is_dir || ssentry->is_empty) {
		printf("                                ");
	} else {
		print_md5_sum(ssentry->hash);
	}
	printf("\t%s\n", ssentry->path);

	if (ssentry->next != NULL) {
		output_ssentries(ssentry->next);
	}
}

SSENTRY* new_ssentry(SNAPSHOT* ss) {
	SSENTRY* ssentry = NULL;

	ssentry = malloc(sizeof(SSENTRY));
	ssentry->next = NULL;
	ss->last->next = ssentry;
	ss->last = ssentry;

	return ssentry;
}

void hash_file(char* path, off_t size, unsigned char* result) {
	int fd;
	char* file_buffer;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		printf("Cannot read file %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}

    file_buffer = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	if (file_buffer == MAP_FAILED) {
		printf("Cannot mmap() file %s (%lld bytes): %s\n", path, (long long) size, strerror(errno));
		exit(EXIT_FAILURE);
	}

    MD5((unsigned char*) file_buffer, size, result);
	if (munmap(file_buffer, size) < 0) {
		printf("Cannot munmap() file %s (%lld bytes): %s\n", path, (long long) size, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (close(fd) < 0) {
		printf("Cannot close file %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

// Returns 1 if the dir is empty.
int process_dir(char* path, SNAPSHOT* snapshot) {
	DIR* dir = NULL;
	int is_empty = 0;

	printf("Processing dir: %s\n", path);
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

		ssentry = new_ssentry(snapshot);
		strncpy(ssentry->path, entry_path, PATH_MAX);

		if (S_ISDIR(entry_info.st_mode)) {
			ssentry->is_dir = 1;
			ssentry->is_empty = process_dir(entry_path, snapshot);
			if (is_empty) {
				is_empty = 0;
			}
		} else if (S_ISREG(entry_info.st_mode)) {
			ssentry->is_dir = 0;
			if (entry_info.st_size > 0) {
				ssentry->is_empty = 0;
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
	ss->first = malloc(sizeof(SSENTRY));
	ss->first->next = NULL;
	ss->last = ss->first;

	process_dir(path, ss);

	return ss;
}

int main(int argc, char* argv[]) {
	SNAPSHOT* ss1;
	SNAPSHOT* ss2;
	
	ss1 = create_snapshot("test1");
	output_ssentries(ss1->first);

	ss2 = create_snapshot("/home/d-ash/distr");
	output_ssentries(ss2->first);

	free(ss1);
	free(ss2);

	return 0;
}

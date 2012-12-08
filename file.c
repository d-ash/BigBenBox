#include "file.h"

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



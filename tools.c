#include "snapshot.h"
#include "tools.h"

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

/*
// hand made hashing
uint16_t uint16_hash(unsigned char* buf, int len) {
	uint16_t res = 0xBABE;
	uint16_t word;

	while (--len >= 0) {
		word = (len & 0x0001) ? buf[len] : (buf[len] << 8);
		res ^= word;
	}

	return res;
}
*/

// SDBM hashing algorithm
uint16_t uint16_hash(unsigned char* buf, int len) {
	uint16_t hash = 0;
	uint16_t word;

	while (--len >= 0) {
		word = (uint16_t) buf[len];
		hash = word + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

void print_hex(unsigned char* md, int len) {
    int i;

    for (i = 0; i < len; i++) {
		printf("%02x", md[i]);
    }
}

#include "snapshot.h"
#include "tools.h"

// SDBM hashing algorithm
uint32_t uint32_hash(void* buf, size_t len) {
	uint32_t hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = (uint32_t) ((unsigned char*) buf)[i] + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

// SDBM hashing algorithm
// the result of this is equivalent to (uint32_hash(buf, len) & 0x0000FFFF) 
uint16_t uint16_hash(void* buf, size_t len) {
	uint16_t hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = (uint16_t) ((unsigned char*) buf)[i] + (hash << 6) - hash;
	}

	return hash;
}

// Streaming version of SDBM-32
// Accepts and returns hash value through parameters.
void update_checksum(void* buf, size_t len, checksum_t* checksum) {
	uint32_t hash;
	size_t i;

	hash = (*checksum);

	for (i = 0; i < len; i++) {
		hash = (uint32_t) ((unsigned char*) buf)[i] + (hash << 6) + (hash << 16) - hash;
	}

	(*checksum) = hash;
}

// network order (big-endian)
void print_hex(void* b, size_t len) {
    size_t i;

    for (i = 0; i < len; i++) {
		printf("%02x", ((unsigned char*) b)[i]);
    }
}

int is_little_endian() {
	static const int i = 1;
	return (* (char*) &i == 1);
}

int sha256_file(char* path, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    FILE* f;
    SHA256_CTX sha;
	char* buf = NULL;
    size_t len = 32768;
	size_t was_read = 0;
	
	f = fopen(path, "rb");
    if (f == NULL) {
		PERR("Cannot open file %s in %s: %s\n", path, __FUNCTION__, strerror(errno));
		return 0;
	}

    SHA256_Init(&sha);

    buf = malloc(len);
    if (buf == NULL) {
		PERR("Cannot allocate memory for SHA256 buffer: %s\n", strerror(errno));
		fclose(f);
		return 0;
	}

    while (was_read = fread(buf, 1, len, f)) {
        SHA256_Update(&sha, buf, was_read);
    }

    SHA256_Final(hash, &sha);

	if (ferror(f)) {
		PERR("Cannot read the file %s: %s\n", path, strerror(errno));
		free(buf);
		fclose(f);
		return 0;
	}

    free(buf);
    fclose(f);
    return 1;
}

#include "snapshot.h"
#include "tools.h"

// SDBM hashing algorithm
uint16_t uint16_hash(void* buf, int len) {
	uint16_t hash = 0;
	uint16_t word;

	while (--len >= 0) {
		word = (uint16_t) ((unsigned char*) buf)[len];
		hash = word + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

void print_hex(void* b, int len) {
    int i;

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

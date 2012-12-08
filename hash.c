#include "hash.h"

/*
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

void print_md5_sum(unsigned char* md) {
    int i;

    for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		printf("%02x", md[i]);
    }
}

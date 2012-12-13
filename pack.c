#include "pack.h"

unsigned char* pack_snapshot(SNAPSHOT* ss) {
	unsigned char* data = NULL;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;
	unsigned char* ptr = NULL;
	size_t chunk = 0;

	data = malloc(ss->entries_count * sizeof(SSENTRY_HEADER) + ss->pathmem_total);
	ptr = data;

	ht = ss->ht_by_hash;
	while (ht != NULL) {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			chunk = sizeof(SSENTRY_HEADER) + ssentry->header.pathmem;
			memcpy(ptr, &ssentry->header, chunk);	// coping header and path
			ptr += chunk;
			ssentry = ssentry->next_by_hash;
		}
		ht = ht->next;
	}

	return data;
}

SNAPSHOT* unpack_snapshot(unsigned char* data) {
	return NULL;
}

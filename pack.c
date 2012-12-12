#include "pack.h"

// Returns new position of the pointer.
unsigned char* pack_entry(SSENTRY* ssentry, unsigned char* ptr) {
	// TODO status_byte
	// unified size of data types

	ptr[0] = 0xFF;
	ptr += 1;
	memcpy(ptr, ssentry->hash, HASH_SUM_LENGTH);
	ptr += HASH_SUM_LENGTH;
	memcpy(ptr, & ssentry->size, sizeof(off_t));
	ptr += sizeof(off_t);
	memcpy(ptr, & ssentry->path_mem, sizeof(size_t));
	ptr += sizeof(size_t);
	memcpy(ptr, ssentry->path, ssentry->path_mem);
	ptr += ssentry->path_mem;

	return ptr;
}

unsigned char* pack_snapshot(SNAPSHOT* ss) {
	unsigned char* data = NULL;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;
	unsigned char* ptr = NULL;

	data = malloc(ss->entries_count * PACK_HEADER_SIZE + ss->path_total_mem);
	ptr = data;

	ht = ss->ht_by_hash;
	while (ht != NULL) {
		ssentry = ht->entries;
		while (ssentry != NULL) {
			ptr = pack_entry(ssentry, ptr);
			ssentry = ssentry->next_by_hash;
		}
		ht = ht->next;
	}

	return data;
}

SNAPSHOT* unpack_snapshot(unsigned char* data) {
}

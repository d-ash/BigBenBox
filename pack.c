#include "pack.h"

// Returns new position of the pointer,
// just after the end of this entry;
unsigned char* pack_entry(SSENTRY* ssentry, unsigned char* ptr) {
	PACK_ENTRY_HEADER* h;

	h = (PACK_ENTRY_HEADER*) ptr;

	// TODO status_byte
	h->status = 0xFFFFFFFF;
	memcpy(&h->hash, ssentry->hash, HASH_SUM_LENGTH);
	h->fsize = ssentry->size;
	h->pmem = ssentry->path_mem;

	ptr += sizeof(PACK_ENTRY_HEADER);
	memcpy(ptr, ssentry->path, ssentry->path_mem);

	return ptr + ssentry->path_mem;
}

unsigned char* pack_snapshot(SNAPSHOT* ss) {
	unsigned char* data = NULL;
	SSHASHTABLE* ht = NULL;
	SSENTRY* ssentry = NULL;
	unsigned char* ptr = NULL;

	data = malloc(ss->entries_count * sizeof(PACK_ENTRY_HEADER) + ss->path_total_mem);
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

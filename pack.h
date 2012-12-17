#ifndef _PACK_H
#define _PACK_H

#include "snapshot.h"

/**
 *	Structure of a packed snapshot after a file header:
 *
 *	[ list of hash values
 *		sizeof(hash_t)			hash value
 *		sizeof(size_t)			size of the following list of entries
 *
 *		[ list of entries
 *			sizeof(SSENTRY)			SSENTRY (SSENTRY.next has dummy value, we recognize NULL values only)
 *			SSENTRY.pathmem			path
 *
 *			...
 *		]
 *
 *		...
 *	]
 *
 *
 */

#define PACKFILE_MAGIC			0x8D
#define PACKFILE_STRUCT_VER		0x01
#define PACKFILE_HEADER_SIZE	4

// WARNING: do not read/write the struct.
// Memory packing takes place!
// This struct is here only for information purposes.
struct s_packfile_header {
	uint8_t magic;
	uint8_t runtime;		// endianess | WORD_SIZE
	uint8_t platform;
	uint8_t pf_st_ver;		// version of a packfile structure
};

typedef struct s_pack_hash_header {
	hash_t hash;
	size_t size;			// memory size used by the following list of entries
} PACK_HASH_HEADER;

int save_snapshot(char* path, SNAPSHOT* ss);
int load_snapshot(char* path, SNAPSHOT* ss);

int pack_snapshot(FILE* f, SNAPSHOT* ss);
int unpack_snapshot(FILE* f, SNAPSHOT* ss);

#endif	// _PACK_H

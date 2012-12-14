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
 *			sizeof(size_t)			dummy value for malloc optimization, used for SSENTRY.next
 *			sizeof(SSENTRY_HEADER)	SSENTRY.header
 *			header.pathmem			path
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
#define PACKFILE_LITTLE_END		0x01
#define PACKFILE_HEADER_SIZE	16

typedef struct s_packfile_header {
	uint8_t magic;
	uint8_t runtime;		// endianess | sizeof(size_t)
	uint16_t platform;
	uint16_t version;
	uint32_t reserved1;
	uint32_t reserved2;
	uint16_t reserved3;
} PACKFILE_HEADER;
// WARNING: do not use sizeof(PACKFILE_HEADER)

typedef struct s_pack_hash_header {
	hash_t hash;
	size_t size;
} PACK_HASH_HEADER;

int save_snapshot(SNAPSHOT* ss, char* path);
SNAPSHOT* load_snapshot(char* path);

#endif	// _PACK_H

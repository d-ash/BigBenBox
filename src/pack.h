#ifndef _BBB_PACK_H
#define _BBB_PACK_H

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

/**
 *	WARNING: do not read/write C-structs, it will be not cross-platform.
 *	Memory packing takes place!
 *
 *		struct s_packfile_header {
 *			uint8_t magic;
 *			uint8_t runtime;		// endianess | WORD_SIZE
 *			uint8_t platform;
 *			uint8_t pf_st_ver;		// version of a packfile structure
 *		};
 */

// In the following struct we use platform dependant types!
typedef struct s_packfile_header_ext {
	hash_t hash_count;			// count of written hashes
	size_t tf_pathmem;			// strlen(tf_path) + 1
} PACKFILE_HEADER_EXT;
// tf_path where this snapshot was taken from

typedef struct s_pack_hash_header {
	hash_t hash;
	size_t size;			// memory size used by the following list of entries
} PACK_HASH_HEADER;

void construct_pfh(unsigned char pfh[PACKFILE_HEADER_SIZE]);

int save_snapshot(char* path, SNAPSHOT* ss);
int load_snapshot(char* path, SNAPSHOT* ss);

int pack_snapshot(FILE* f, SNAPSHOT* ss, bbbChecksum_t* checksum_p);
int unpack_snapshot(FILE* f, SNAPSHOT* ss, bbbChecksum_t* checksum_p);

#endif

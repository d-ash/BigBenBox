/**
 *	Structure of a packed snapshot after a file header:
 *
 *	[ list of hash values
 *		sizeof( bbbSsHash_t )			hash value
 *		sizeof( size_t )				size of the following list of entries
 *
 *		[ list of entries
 *			sizeof( bbbSsEntry_t )		bbbSsEntry_t (bbbSsEntry_t.next has dummy value, we recognize NULL values only)
 *			bbbSsEntry_t.pathmem		path
 *
 *			...
 *		]
 *
 *		...
 *	]
 */

/**
 *	WARNING: do not read/write C-structs, it will be not cross-platform.
 *	Memory packing takes place!
 *
 *		struct bbbPfHeader_s {
 *			uint8_t	magic;
 *			uint8_t	runtime;		// endianess | WORD_SIZE
 *			uint8_t	platform;
 *			uint8_t	pf_st_ver;		// version of a packfile structure
 *		};
 */

#ifndef _BBB_PACK_H
#define _BBB_PACK_H

#include "snapshot.h"

#define BBB_PF_MAGIC			0x8D
#define BBB_PF_STRUCT_VER		0x01
#define BBB_PF_HEADER_SIZE		4

// In the following struct we use platform dependant types!
typedef struct bbbPfHeaderExt_s {
	bbbSsHash_t		hash_count;		// count of written hashes
	size_t			tf_pathmem;		// strlen( tf_path ) + 1
} bbbPfHeaderExt_t;
/**
 * tf_path where this snapshot was taken from
 */

typedef struct bbbPfHashHeader_s {
	bbbSsHash_t		hash;
	size_t			size;			// memory size used by the following list of entries
} bbbPfHashHeader_t;

// =================== Exported functions ===================

int		BbbSaveSnapshot( const char* const path, const bbbSnapshot_t* const ss );

int		BbbLoadSnapshot( const char* const path, bbbSnapshot_t* const ss );

#endif

/**
 *	Structure of a saved snapshot after a file header:
 *
 *	[ list of hash values
 *		sizeof( bbbSsHash_t )			hash value
 *		sizeof( size_t )				size of the following list of entries
 *
 *		[ list of entries
 *			sizeof( bbbSsEntry_t )		bbbSsEntry_t (bbbSsEntry_t.next has dummy value, we recognize NULL values only)
 *			bbbSsEntry_t.pathMem		path
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
 *		struct bbbSsFileHdr_s {
 *			uint8_t	magic;
 *			uint8_t	runtime;		// endianess | WORD_SIZE
 *			uint8_t	platform;
 *			uint8_t	ssFileVersion;	// version of a file structure
 *		};
 */

#ifndef _BBB_SNAPSHOTFILE_H
#define _BBB_SNAPSHOTFILE_H

#include "snapshot.h"

#define BBB_SS_FILE_MAGIC		0x8D
#define BBB_SS_FILE_VERSION		0x01
#define BBB_SS_FILE_HDR_SIZE	4

// In the following struct we use platform dependant types!
typedef struct bbbSsFileHdrExt_s {
	size_t			takenFromMem;	// strlen( bbbSnapshot_t.takenFrom ) + 1
} bbbSsFileHdrExt_t;
/**
 * bbbSnapshot_t.takenFrom will be stored here, just after bbbSsFileHdrExt_t.
 */

typedef struct bbbSsFileHashHdr_s {
	bbbSsHash_t		hash;
	size_t			size;			// memory size used by the following list of entries
} bbbSsFileHashHdr_t;

// =================== Exported functions ===================

int		BbbSaveSnapshot( const char* const path, const bbbSnapshot_t* const ss );

int		BbbLoadSnapshot( const char* const path, bbbSnapshot_t* const ss );

#endif

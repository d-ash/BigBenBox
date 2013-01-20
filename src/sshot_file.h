#ifndef _BBB_SSHOT_FILE_H
#define _BBB_SSHOT_FILE_H

#include "sshot.h"

#define BBB_SSHOT_FILE_MAGIC		0x8D
#define BBB_SSHOT_FILE_VERSION		0x01
#define BBB_SSHOT_FILE_HDR_SIZE		4

/**
 *	Structure of a saved snapshot after a file header:
 *
 *	[ list of hash values
 *		sizeof( bbb_sshot_hash_t )		- hash value
 *		sizeof( size_t )				- size of the following list of entries
 *
 *		[ list of entries
 *			sizeof( bbb_sshot_entry_t )	- bbbSsEntry_t (bbbSsEntry_t.next has dummy value, we recognize NULL values only)
 *			bbb_sshot_entry_t.pathMem	- path
 *
 *			...
 *		]
 *
 *		...
 *	]
 *
 *	WARNING: do not read/write C-structs, it will be not cross-platform.
 *	Memory packing takes place!
 *
 *		struct bbb_sshot_file_hdr {
 *			uint8_t	magic;
 *			uint8_t	runtime;		// ( endianess | WORD_SIZE )
 *			uint8_t	platform;
 *			uint8_t	ssFileVersion;	// version of a file structure
 *		};
 */

// In the following structs we use platform dependant types!
typedef struct {
	size_t	takenFromMem;			// strlen( bbb_sshot_t.takenFrom ) + 1
} bbb_sshot_file_hdr2_t;			// second (extended) header
/**
 *  bbb_sshot_t.takenFrom will be stored here, just after the struct.
 */

typedef struct {
	bbb_sshot_hash_t	hash;
	size_t				size;		// memory size used by the following list of entries
} bbb_sshot_file_ht_t;


// =================== Exported functions ===================

int		bbb_sshot_file_Save( const char* const path, const bbb_sshot_t* const ss );

int		bbb_sshot_file_Load( const char* const path, bbb_sshot_t* const ss );

#endif

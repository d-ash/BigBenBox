<?:include	bbb.p ?>
<?:prefix	@_ bbb_sshot_file_ ?>
<?:prefix	@^ BBB_SSHOT_FILE_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "bbb_sshot.h"
#include "bbb_sshot_file.bio.h"

#define @^MAGIC			0x8D
#define @^FORMAT		0x01

/**
 *	Structure of a saved snapshot after a file header:
 *
 *	[ list of hash values
 *		sizeof( bbb_sshot_hash_t )		- hash value
 *		sizeof( size_t )				- size of the following list of entries
 *		[ list of entries
 *			sizeof( bbb_sshot_entry_t )	- bbb_sshot_entry_t (.next has a dummy value, we recognize NULL values only)
 *			bbb_sshot_entry_t.pathMem	- path
 *			...
 *		]
 *		...
 *	]
 *
 *	WARNING: do not read/write C-structs, it will be not cross-platform.
 *	Compilers are aligning memory!
 */

// In the following structs we use platform dependant types!
typedef struct {
	size_t	takenFromMem;		// strlen( bbb_sshot_t.takenFrom ) + 1
} @_hdr2_t;						// second (extended) header, machine specific
/**
 *	bbb_sshot_t.takenFrom will be stored here, just after the struct.
 */

typedef struct {
	bbb_sshot_hash_t	hash;
	size_t				size;	// memory size used by the following list of entries
} @_ht_t;

// =================== Exported functions ===================

bbb_result_t	@_Save( const char* const path, const bbb_sshot_t* const ss );
bbb_result_t	@_Load( const char* const path, bbb_sshot_t* const ss );

#endif

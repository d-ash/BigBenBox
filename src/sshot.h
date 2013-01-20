/**
 *  Snapshot can be one of two types: restored or generated manually.
 *  Restored snapshot are a special case when memory for entries
 *  is allocated in one huge chunk (at bbb_sshot_t.ht->first).
 *  So destroying must be kept according to the flag bbb_sshot_t.restored.
 *  And adding to this kind of snapshots is prohibited.
 *
 *  In the future this can be reimplemented, so that every entry would have a separate flag.
 *  This will allow to manipulate with restored snapshots as with normal.
 *  And it will be possible to compact a memory usage by sequential saving & loading of snapshot.
 */

#ifndef _BBB_SSHOT_H
#define _BBB_SSHOT_H

#include "global.h"

typedef uint16_t			bbb_sshot_hash_t;

#define BBB_SSHOT_HASH_MAX	UINT16_MAX

typedef struct {
	time_t	mtime;
	off_t	size;
} bbb_sshot_content_t;

typedef struct {
	uint8_t					status;
	uint8_t					custom;		// can be used by used code, at the end it has to be reset to 0
	bbb_sshot_content_t		content;
	size_t					pathMem;	// strlen( path ) + 1 + additional bytes for memory alignment
	void*					next;		// link to the next bbb_sshot_entry_t
} bbb_sshot_entry_t;

/**
 *  WARNING: 'path' is stored here, just after bbb_sshot_entry_t.
 *  sizeof( bbb_sshot_entry_t ) must be multiple of BBB_WORD_SIZE.
 */

#define BBB_SSHOT_ENTRY_PATH( s )		( ( char* ) s + sizeof( bbb_sshot_entry_t ) )
#define BBB_SSHOT_ENTRY_STATUS_DIR		0x01

typedef struct {
	bbb_sshot_entry_t*	first;
	size_t				size;			// total memory used by all entries of this hash value (with paths)
} bbb_sshot_ht_t;

typedef struct {
	int					restored;		// 0: dynamically generated, 1: restored from file
	char*				takenFrom;		// path where this snapshot was taken from
	bbb_sshot_ht_t*		ht;				// hashtable
} bbb_sshot_t;

// ================= Exported functions =================

					// does not allocate memory for bbb_sshot_t
int					bbb_sshot_Init( bbb_sshot_t* const ss );

					// does not free bbb_sshot_t itself
int					bbb_sshot_Destroy( bbb_sshot_t* const ss );

int					bbb_sshot_Take( const char* const path, bbb_sshot_t* const ss );

bbb_sshot_entry_t*	bbb_sshot_Search( const char* const path, const bbb_sshot_t* const ss );

int					bbb_sshot_Diff( const bbb_sshot_t* const ss0, const bbb_sshot_t* const ss1 );

#endif

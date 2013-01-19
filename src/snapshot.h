/**
 *  Snapshot can be one of two types: restored or generated manually.
 *  Restored snapshot are a special case when memory for entries
 *  is allocated in one huge chunk (at bbbSnapshot_t.ht->first).
 *  So destroying must be kept according to the flag bbbSnapshot_t.restored.
 *  And adding to this kind of snapshots is prohibited.
 *
 *  In the future this can be reimplemented, so that every entry would have a separate flag.
 *  This will allow to manipulate with restored snapshots as with normal.
 *  And it will be possible to compact a memory usage by sequential saving & loading of snapshot.
 */

#ifndef _BBB_SNAPSHOT_H
#define _BBB_SNAPSHOT_H

#include "global.h"

typedef uint16_t			bbbSsHash_t;

#define BBB_SS_HASH_MAX		UINT16_MAX

typedef struct bbbSsEntryContent_s {
	time_t	mtime;
	off_t	size;
} bbbSsEntryContent_t;

typedef struct bbbSsEntry_s {
	uint8_t				status;
	uint8_t				custom;			// can be used by used code, at the end it has to be reset to 0
	bbbSsEntryContent_t	content;
	size_t				pathMem;		// strlen( path ) + 1 + additional bytes for memory alignment
	void*				next;			// link to the next bbbSsEntry_t
} bbbSsEntry_t;

/**
 *  WARNING: 'path' is stored here, just after bbbSsEntry_t.
 *  sizeof( bbbSsEntry_t ) must be multiple of BBB_WORD_SIZE.
 */

#define BBB_SS_ENTRY_PATH( s )		( ( char* ) s + sizeof( bbbSsEntry_t ) )
#define BBB_SS_ENTRY_STATUS_DIR		0x01

typedef struct bbbSsHashHdr_s {
	bbbSsEntry_t*	first;
	size_t			size;				// total memory used by all entries of this hash value (with paths)
} bbbSsHashHdr_t;

typedef struct bbbSnapshot_s {
	int				restored;			// 0: dynamically generated, 1: restored from file
	char*			takenFrom;			// path where this snapshot was taken from
	bbbSsHashHdr_t*	ht;					// hashtable
} bbbSnapshot_t;

// ================= Exported functions =================

				// does not allocate memory for bbbSnapshot_t
int				BbbInitSnapshot( bbbSnapshot_t* const ss );

				// does not free bbbSnapshot_t itself
int				BbbDestroySnapshot( bbbSnapshot_t* const ss );

int				BbbTakeSnapshot( const char* const path, bbbSnapshot_t* const ss );

bbbSsEntry_t*	BbbSearchSnapshot( const char* const path, const bbbSnapshot_t* const ss );

int				BbbDiffSnapshot( const bbbSnapshot_t* const ss0, const bbbSnapshot_t* const ss1 );

#endif

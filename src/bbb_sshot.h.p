<?:include bbb.p ?>
<?:prefix @_ bbb_sshot_ ?>
<?:prefix @^ BBB_SSHOT_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "bbb.h"

/**
 *	Snapshot can be one of two types: restored or generated manually.
 *	Restored snapshot are a special case when memory for entries
 *	is allocated in one huge chunk (at @_t.ht->first).
 *	So destroying must be kept according to the flag @_t.restored.
 *	And adding to this kind of snapshots is prohibited.
 *
 *	In the future this can be reimplemented, so that every entry would have a separate flag.
 *	This will allow to manipulate with restored snapshots as with normal.
 *	And it will be possible to compact a memory usage by sequential saving & loading of snapshot.
 */

typedef uint16_t		@_hash_t;

#define @^HASH_MAX		UINT16_MAX

typedef struct {
	time_t		mtime;
	off_t		size;
} @_content_t;

typedef struct {
	uint8_t			status;
	uint8_t			custom;		// can be used by used code, at the end it has to be reset to 0
	@_content_t		content;
	size_t			pathMem;	// strlen( path ) + 1 + additional bytes for memory alignment
	void*			next;		// link to the next @_entry_t
} @_entry_t;

/**
 *	WARNING: 'path' is stored here, just after @_entry_t.
 *	sizeof( @_entry_t ) must be multiple of BBB_WORD_SIZE.
 */

#define @^ENTRY_PATH( s )		( ( char* ) s + sizeof( @_entry_t ) )
#define @^ENTRY_STATUS_DIR		0x01

typedef struct {
	@_entry_t*	first;
	size_t		size;			// total memory used by all entries of this hash value (with paths)
} @_ht_t;

typedef struct {
	int			restored;		// 0: dynamically generated, 1: restored from file
	char*		takenFrom;		// path where this snapshot was taken from
	@_ht_t*		ht;				// hashtable
} @_t;

// ================= Exported functions =================

				// does not allocate memory for @_t
bbb_result_t	@_Init( @_t* const ss );

				// does not free @_t itself
void			@_Destroy( @_t* const ss );

bbb_result_t	@_Take( const char* const path, @_t* const ss );

@_entry_t*		@_Search( const char* const path, const @_t* const ss );

int				@_Diff( const @_t* const ss0, const @_t* const ss1 );

#endif

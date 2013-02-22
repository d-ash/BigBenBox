<?:prefix @_ bbb_repo_ ?>
<?:prefix @^ BBB_REPO_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "global.h"

#define @^_DIR	"s/repos"

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
int				@_Init( @_t* const ss );

				// does not free @_t itself
int				@_Destroy( @_t* const ss );

int				@_Take( const char* const path, @_t* const ss );

@_entry_t*		@_Search( const char* const path, const @_t* const ss );

int				@_Diff( const @_t* const ss0, const @_t* const ss1 );

#endif

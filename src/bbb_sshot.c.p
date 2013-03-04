<?:include bbb.p ?>
<?:prefix @_ bbb_sshot_ ?>
<?:prefix @^ BBB_SSHOT_ ?>

#include "bbb_sshot.h"
#include "bbb_util.h"
#include "bbb_util_hash.h"

static bbb_result_t		_ProcessDir( const char* const path, const size_t skip, @_t* const ss );
static bbb_result_t		_ProcessEntry( const char* const path, const size_t skip, const char* const name, @_t* const ss );
static bbb_result_t		_AddToSnapshot( @_entry_t* const entry, @_t* const ss );

bbb_result_t
@_Init( @_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;

	ss->restored = 0;				// by default a snapshot is 'generated'
	ss->takenFrom = NULL;
	<? bbb_Call( "?> bbb_util_Malloc( ( void** )&( ss->ht ), sizeof( @_ht_t ) * @^HASH_MAX ) <?" ); ?>

	// assuming NULL == 0
	memset( ss->ht, 0, sizeof( @_ht_t ) * @^HASH_MAX );

	<? c_Cleanup(); ?>
	return result;
}

void
@_Destroy( @_t* const ss ) {
	@_hash_t	i;
	@_entry_t*	entry = NULL;
	void*		mustDie = NULL;

	for ( i = 0; i < @^HASH_MAX; i++ ) {
		entry = ss->ht[ i ].first;
		if ( entry != NULL ) {
			if ( ss->restored ) {
				free( entry );		// all entries at once
			} else {
				do {
					mustDie = entry;
					entry = entry->next;
					free( mustDie );
				} while ( entry != NULL );
			}
		}
	}

	free( ss->ht );
	ss->ht = NULL;

	if ( ss->takenFrom != NULL ) {
		free( ss->takenFrom );
	}
}

bbb_result_t
@_Take( const char* const path, @_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;
	size_t			len = 0;

	<? bbb_Call( "?> @_Init( ss ) <?" ); ?>
	<? c_OnCleanup( "?>
		if ( BBB_FAILED( result ) ) {
			@_Destroy( ss );
		}
	<?" ); ?>

	// this memory will be released in @_Destroy() if not NULL
	len = strlen( path );
	<? bbb_Call( "?> bbb_util_Strdup( path, &( ss->takenFrom ) ) <?" ); ?>

	// trim slash at the end if necessary
	if ( len > 1 && ( ss->takenFrom[ len - 1 ] == '/' || ss->takenFrom[ len - 1 ] == '\\' ) ) {
		len--;
		ss->takenFrom[ len ] = 0;
	}
	<? bbb_Call( "?> _ProcessDir( ss->takenFrom, len, ss ) <?" ); ?>

	<? c_Cleanup(); ?>
	return result;
}

@_entry_t*
@_Search( const char* const path, const @_t* const ss ) {
	@_hash_t	hash;
	@_entry_t*	entry = NULL;

	hash = bbb_util_hash_Calc_uint16( path, strlen( path ) );
	entry = ss->ht[ hash ].first;
	while ( entry != NULL ) {
		if ( strcmp( path, @^ENTRY_PATH( entry ) ) == 0 ) {
			break;
		}
		entry = entry->next;
	}

	return entry;
}

int
@_Diff( const @_t* const ss0, const @_t* const ss1 ) {
	@_hash_t	i;
	@_entry_t*	entry = NULL;
	@_entry_t*	found = NULL;
	char*		path = NULL;
	int			differs = 0;

	for ( i = 0; i < @^HASH_MAX; i++ ) {
		entry = ss1->ht[ i ].first;

		while ( entry != NULL ) {
			path = @^ENTRY_PATH( entry );
			found = @_Search( path, ss0 );

			if ( found == NULL ) {
				if ( differs == 0 ) {
					differs = 1;
				}
				printf( "CHANGE_2: %s\n", path );
			} else {
				found->custom = 1;		// not to be checked at the loop over ss0
				if ( found->status != entry->status
						|| found->content.mtime != entry->content.mtime
						|| found->content.size != entry->content.size ) {
					if ( differs == 0 ) {
						differs = 1;
					}
					printf( "CHANGE_1: %s\n", path );
				}
			}

			entry = entry->next;
		}
	}

	for ( i = 0; i < @^HASH_MAX; i++ ) {
		entry = ss0->ht[ i ].first;

		while ( entry != NULL ) {
			if ( entry->custom == 0 ) {
				path = @^ENTRY_PATH( entry );
				found = @_Search( path, ss1 );

				if ( found == NULL ) {
					if ( differs == 0 ) {
						differs = 1;
					}
					printf( "CHANGE_3: %s\n", path );
				}
			} else {
				entry->custom = 0;		// resetting to a default value
			}

			entry = entry->next;
		}
	}

	return differs;
}

// ================= Static ===============

static bbb_result_t
_ProcessDir( const char* const path, const size_t skip, @_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;
	DIR*			dir = NULL;
	struct dirent*	entry = NULL;

	BBB_LOG( "Processing dir: %s", path );
	dir = opendir( path );
	if ( dir == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "Cannot open dir %s: %s", path, strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		if ( closedir( dir ) < 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "Cannot close dir %s: %s", path, strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
	<?" ); ?>

	while ( 1 ) {
		// Windows port dirent.h does not have readdir_r().
		// Fortunately we don't need it here.
		entry = readdir( dir );
		if ( entry == NULL ) {
			break;							// an error or the end of the directory
		}

		// skipping "." and ".."
		if ( entry->d_name[ 0 ] == '.' ) {
			if ( entry->d_name[ 1 ] == '\0' ||
					( entry->d_name[ 1 ] == '.' &&
						entry->d_name[ 2 ] == '\0' ) ) {
				continue;
			}
		}

		<? bbb_Call( "?> _ProcessEntry( path, skip, entry->d_name, ss ) <?" ); ?>
	}

	<? c_Cleanup(); ?>
	return result;
}

static bbb_result_t
_ProcessEntry( const char* const path, const size_t skip, const char* const name, @_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;
	struct stat		entryInfo;
	@_entry_t*		entry = NULL;
	size_t			pathMem = 0;
	char*			fullPath = NULL;		// path with a root dir of this processing

	// allocating memory for @_entry_t + path, pathMem will be aligned to BBB_WORD_SIZE
	// in order to get properly aligned memory after loading this data from a file.
	pathMem = ( strlen( path ) - skip + strlen( name ) + 1 + BBB_WORD_SIZE ) & ~( BBB_WORD_SIZE - 1 );

	<? bbb_Call( "?> bbb_util_Malloc( ( void** )&entry, sizeof( @_entry_t ) + pathMem ) <?" ); ?>
	<? c_OnCleanup( "?>
		if ( BBB_FAILED( result ) ) {
			free( entry );
		}
	<?" ); ?>

	<? bbb_Call( "?> bbb_util_Malloc( ( void** )&fullPath, pathMem + skip + 1 ) <?" ); ?>
	<? c_OnCleanup( "?>
		free( fullPath );
	<?" ); ?>

	strcpy( fullPath, path );
	strcat( fullPath, "/" );
	strcat( fullPath, name );

	entry->status = 0;
	entry->custom = 0;
	entry->pathMem = pathMem;
	strcpy( @^ENTRY_PATH( entry ), fullPath + skip + 1 );

	if ( stat( fullPath, &entryInfo ) ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "Cannot get info about %s: %s", fullPath, strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}

	entry->content.size = entryInfo.st_size;
	entry->content.mtime = entryInfo.st_mtime;

	if ( S_ISDIR( entryInfo.st_mode ) ) {
		entry->status |= @^ENTRY_STATUS_DIR;
		<? bbb_Call( "?> _AddToSnapshot( entry, ss ) <?" ); ?>
		<? bbb_Call( "?> _ProcessDir( fullPath, skip, ss ) <?" ); ?>
	} else if ( S_ISREG( entryInfo.st_mode ) ) {
		entry->status &= ~@^ENTRY_STATUS_DIR;
		<? bbb_Call( "?> _AddToSnapshot( entry, ss ) <?" ); ?>
	} else {
		BBB_LOG( "Skipping irregular file: %s", fullPath );
		<? c_GotoCleanup(); ?>
	}

	<? c_Cleanup(); ?>
	return result;
}

static bbb_result_t
_AddToSnapshot( @_entry_t* const entry, @_t* const ss ) {
	@_hash_t	hash;

	if ( ss->restored ) {
		BBB_ERR_CODE( BBB_ERROR_DEVELOPER, "Adding entries to restored snapshots is prohibited" );
		return BBB_ERROR_DEVELOPER;
	}

	hash = bbb_util_hash_Calc_uint16( @^ENTRY_PATH( entry ), strlen( @^ENTRY_PATH( entry ) ) );

	// push to the beginning of the list
	entry->next = ss->ht[ hash ].first;
	ss->ht[ hash ].first = entry;
	ss->ht[ hash ].size += sizeof( @_entry_t ) + entry->pathMem;

	return BBB_SUCCESS;
}

#include "sshot.h"
#include "util_hash.h"

static int	_ProcessDir( const char* const path, const size_t skip, bbb_sshot_t* const ss );
static int	_ProcessEntry( const char* const path, const size_t skip, const char* const name, bbb_sshot_t* const ss );
static int	_AddToSnapshot( bbb_sshot_entry_t* const entry, bbb_sshot_t* const ss );

int bbb_sshot_Init( bbb_sshot_t* const ss ) {
	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	ss->restored = 0;		// by default a snapshot is generated
	ss->takenFrom = NULL;
	ss->ht = malloc( sizeof( bbb_sshot_ht_t ) * BBB_SSHOT_HASH_MAX );

	// assuming NULL == 0
	memset( ss->ht, 0, sizeof( bbb_sshot_ht_t ) * BBB_SSHOT_HASH_MAX );

	return 1;
}

int bbb_sshot_Destroy( bbb_sshot_t* const ss ) {
	bbb_sshot_hash_t	i;
	bbb_sshot_entry_t*	entry = NULL;
	void*				mustDie = NULL;

	if ( ss == NULL || ss->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	for ( i = 0; i < BBB_SSHOT_HASH_MAX; i++ ) {
		entry = ss->ht[ i ].first;

		if ( ss->restored ) {
			if ( entry != NULL ) {
				free( entry );		// all entries at once
			}
		} else {
			while ( entry != NULL ) {
				mustDie = entry;
				entry = entry->next;
				free( mustDie );
			}
		}
	}

	free( ss->ht );
	ss->ht = NULL;

	if ( ss->takenFrom != NULL ) {
		free( ss->takenFrom );
	}

	return 1;
}

int bbb_sshot_Take( const char* const path, bbb_sshot_t* const ss ) {
	int		len = 0;
	char*	p;

	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	bbb_sshot_Init( ss );

	p = strdup( path );
	len = strlen( p );

	// trim slash at the end if necessary
	if ( len > 1 && ( p[ len ] == '/' || p[ len ] == '\\' ) ) {
		len--;
		p[ len ] = 0;
	}

	if ( !_ProcessDir( p, len, ss ) ) {
		bbb_sshot_Destroy( ss );
		free( p );
		return 0;
	}

	ss->takenFrom = p;

	return 1;
}

bbb_sshot_entry_t* bbb_sshot_Search( const char* const path, const bbb_sshot_t* const ss ) {
	bbb_sshot_hash_t	hash;
	bbb_sshot_entry_t*	entry = NULL;
	size_t				pathLen;

	if ( ss == NULL || ss->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return NULL;
	}

	pathLen = strlen( path );
	hash = bbb_util_hash_Calc_uint16( path, pathLen );

	entry = ss->ht[ hash ].first;
	while ( entry != NULL ) {
		if ( strncmp( path, BBB_SSHOT_ENTRY_PATH( entry ), pathLen + 1 ) == 0 ) {
			break;
		}
		entry = entry->next;
	}

	return entry;
}

int bbb_sshot_Diff( const bbb_sshot_t* const ss0, const bbb_sshot_t* const ss1 ) {
	bbb_sshot_hash_t	i;
	bbb_sshot_entry_t*	entry = NULL;
	bbb_sshot_entry_t*	found = NULL;
	char*				path = NULL;
	int					differs = 0;

	if ( ss0 == NULL || ss0->ht == NULL || ss1 == NULL || ss1->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;	// TODO what we need return?
	}

	for ( i = 0; i < BBB_SSHOT_HASH_MAX; i++ ) {
		entry = ss1->ht[ i ].first;

		while ( entry != NULL ) {
			path = BBB_SSHOT_ENTRY_PATH( entry );
			found = bbb_sshot_Search( path, ss0 );

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

	for ( i = 0; i < BBB_SSHOT_HASH_MAX; i++ ) {
		entry = ss0->ht[ i ].first;

		while ( entry != NULL ) {
			if ( entry->custom == 0 ) {
				path = BBB_SSHOT_ENTRY_PATH( entry );
				found = bbb_sshot_Search( path, ss1 );

				if ( found == NULL ) {
					if ( differs == 0 ) {
						differs = 1;
					}
					printf( "CHANGE_3: %s\n", path );
				}
			} else {
				entry->custom = 0;	// resetting to a default value
			}

			entry = entry->next;
		}
	}

	return differs;
}

// ================= Static ===============

static int _ProcessDir( const char* const path, const size_t skip, bbb_sshot_t* const ss ) {
	DIR*			dir = NULL;
	struct dirent*	entry = NULL;
	int				res = 1;

	BBB_PLOG( "Processing dir: %s\n", path );
	dir = opendir( path );
	if ( dir == NULL ) {
		BBB_PERR( "Cannot open dir %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	while ( 1 ) {
		// Windows port dirent.h does not have readdir_r().
		// BTW we don't need it here.
		entry = readdir( dir );
		if ( entry == NULL ) {
			// an error or the end of the directory
			break;
		}

		if ( ( strncmp( entry->d_name, ".", 2 ) == 0 ) || 
			( strncmp( entry->d_name, "..", 3 ) == 0 ) ) {
			continue;
		}

		if ( !_ProcessEntry( path, skip, entry->d_name, ss ) ) {
			res = 0;
			break;
		}
	}

	if ( closedir( dir ) < 0 ) {
		BBB_PERR( "Cannot close dir %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	return res;
}

static int _ProcessEntry( const char* const path, const size_t skip, const char* const name, bbb_sshot_t* const ss ) {
	struct stat			entryInfo;
	bbb_sshot_entry_t*	entry = NULL;
	size_t				pathMem = 0;
	char*				fullPath = NULL;		// path with a root dir of this processing
	size_t				pl = 0;
	size_t				nl = 0;

	pl = strlen( path );
	nl = strlen( name );

	// allocating memory for bbb_sshot_entry_t + path, pathMem will be aligned to BBB_WORD_SIZE
	// in order to get properly aligned memory after loading this data from a file.
	pathMem = ( pl - skip + nl + 1 + BBB_WORD_SIZE ) & ~( BBB_WORD_SIZE - 1 );
	entry = malloc( sizeof( bbb_sshot_entry_t ) + pathMem );

	if ( entry == NULL ) {
		BBB_PERR( "Cannot allocate memory for an entry: %s\n", strerror( errno ) );
		return 0;
	}

	fullPath = malloc( pathMem + skip + 1 );
	strncpy( fullPath, path, pl + 1 );
	strncat( fullPath, "/", 2 );
	strncat( fullPath, name, nl + 1 );

	entry->status = 0;
	entry->custom = 0;
	entry->pathMem = pathMem;
	strncpy( BBB_SSHOT_ENTRY_PATH( entry ), fullPath + skip + 1, pathMem );

	if ( stat( fullPath, &entryInfo ) ) {
		BBB_PERR( "Cannot get info about %s: %s\n", fullPath, strerror( errno ) );
		free( entry );
		free( fullPath );
		return 0;
	}

	entry->content.size = entryInfo.st_size;
	entry->content.mtime = entryInfo.st_mtime;

	if ( S_ISDIR( entryInfo.st_mode ) ) {
		entry->status |= BBB_SSHOT_ENTRY_STATUS_DIR;
		_ProcessDir( fullPath, skip, ss );
	} else if ( S_ISREG( entryInfo.st_mode ) ) {
		entry->status &= ~BBB_SSHOT_ENTRY_STATUS_DIR;
	} else {
		BBB_PLOG( "Skipping irregular file: %s\n", fullPath );
		free( entry );
		free( fullPath );
		return 1;		// it is a successful operation
	}

	free( fullPath );
	return _AddToSnapshot( entry, ss );
}

static int _AddToSnapshot( bbb_sshot_entry_t* const entry, bbb_sshot_t* const ss ) {
	bbb_sshot_hash_t		hash;

	if ( ss->restored ) {
		BBB_PERR( "Adding entries to a restored snapshot is denied." );
		return 0;
	}

	hash = bbb_util_hash_Calc_uint16( BBB_SSHOT_ENTRY_PATH( entry ), strlen( BBB_SSHOT_ENTRY_PATH( entry ) ) );

	// push to the beginning of the list
	entry->next = ss->ht[ hash ].first;
	ss->ht[ hash ].first = entry;
	ss->ht[ hash ].size += sizeof( bbb_sshot_entry_t ) + entry->pathMem;

	return 1;
}
#include <sys/stat.h>

#include "snapshot.h"

static int	_ProcessDir( const char* const path, const size_t skip, bbbSnapshot_t* const ss );
static int	_ProcessEntry( const char* const path, const size_t skip, const char* const name, bbbSnapshot_t* const ss );
static int	_AddToSnapshot( bbbSsEntry_t* const ssentry, bbbSnapshot_t* const ss );

int bbbInitSnapshot( bbbSnapshot_t* const ss ) {
	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	ss->restored = 0;		// by default it's generated
	ss->tf_path = NULL;
	ss->ht = malloc( sizeof( bbbSsHashHeader_t ) * BBB_SS_HASH_MAX );

	// assuming NULL == 0
	memset( ss->ht, 0, sizeof( bbbSsHashHeader_t ) * BBB_SS_HASH_MAX );

	return 1;
}

int bbbDestroySnapshot( bbbSnapshot_t* const ss ) {
	bbbSsHash_t		i;
	bbbSsEntry_t*	ssentry = NULL;
	void*			mustdie = NULL;

	if ( ss == NULL || ss->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	for ( i = 0; i < BBB_SS_HASH_MAX; i++ ) {
		ssentry = ss->ht[ i ].first;

		if ( ss->restored ) {
			if ( ssentry != NULL ) {
				free( ssentry );		// all entries at once
			}
		} else {
			while ( ssentry != NULL ) {
				mustdie = ssentry;
				ssentry = ssentry->next;
				free( mustdie );
			}
		}
	}

	free( ss->ht );
	ss->ht = NULL;

	if ( ss->tf_path != NULL ) {
		free( ss->tf_path );
	}

	return 1;
}

int bbbTakeSnapshot( const char* const path, bbbSnapshot_t* const ss ) {
	int		len = 0;
	char*	p;

	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	bbbInitSnapshot( ss );

	p = strdup( path );
	len = strlen( p );

	// trim slash at the end if necessary
	if ( len > 1 && ( p[ len ] == '/' || p[ len ] == '\\' ) ) {
		len--;
		p[ len ] = 0;
	}

	if ( !_ProcessDir( p, len, ss ) ) {
		bbbDestroySnapshot( ss );
		free( p );
		return 0;
	}

	ss->tf_path = p;

	return 1;
}

bbbSsEntry_t* bbbSearchSnapshot( const char* const path, const bbbSnapshot_t* const ss ) {
	bbbSsHash_t		hash;
	bbbSsEntry_t*	ssentry = NULL;
	size_t			pathlen;

	if ( ss == NULL || ss->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return NULL;
	}

	pathlen = strlen( path );
	hash = bbbHashBuf_uint16( path, pathlen );

	ssentry = ss->ht[ hash ].first;
	while ( ssentry != NULL ) {
		if ( strncmp( path, BBB_SS_ENTRY_PATH( ssentry ), pathlen + 1 ) == 0 ) {
			break;
		}
		ssentry = ssentry->next;
	}

	return ssentry;
}

int bbbDiffSnapshot( const bbbSnapshot_t* const ss0, const bbbSnapshot_t* const ss1 ) {
	bbbSsHash_t		i;
	bbbSsEntry_t*	ssentry = NULL;
	bbbSsEntry_t*	found = NULL;
	char*			path = NULL;
	int				differs = 0;

	if ( ss0 == NULL || ss0->ht == NULL || ss1 == NULL || ss1->ht == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;	// TODO what we need return?
	}

	for ( i = 0; i < BBB_SS_HASH_MAX; i++ ) {
		ssentry = ss1->ht[ i ].first;

		while ( ssentry != NULL ) {
			path = BBB_SS_ENTRY_PATH( ssentry );
			found = bbbSearchSnapshot( path, ss0 );

			if ( found == NULL ) {
				if ( differs == 0 ) {
					differs = 1;
				}
				printf( "CHANGE_2: %s\n", path );
			} else {
				found->custom = 1;		// not to be checked at the loop over ss0
				if ( found->status != ssentry->status
						|| found->content.mtime != ssentry->content.mtime
						|| found->content.size != ssentry->content.size ) {
					if ( differs == 0 ) {
						differs = 1;
					}
					printf( "CHANGE_1: %s\n", path );
				}
			}

			ssentry = ssentry->next;
		}
	}

	for ( i = 0; i < BBB_SS_HASH_MAX; i++ ) {
		ssentry = ss0->ht[ i ].first;

		while ( ssentry != NULL ) {
			if ( ssentry->custom == 0 ) {
				path = BBB_SS_ENTRY_PATH( ssentry );
				found = bbbSearchSnapshot( path, ss1 );

				if ( found == NULL ) {
					if ( differs == 0 ) {
						differs = 1;
					}
					printf( "CHANGE_3: %s\n", path );
				}
			} else {
				ssentry->custom = 0;	// resetting to a default value
			}

			ssentry = ssentry->next;
		}
	}

	return differs;
}

// ================= Static ===============

static int _ProcessDir( const char* const path, const size_t skip, bbbSnapshot_t* const ss ) {
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

static int _ProcessEntry( const char* const path, const size_t skip, const char* const name, bbbSnapshot_t* const ss ) {
	struct stat		entry_info;
	bbbSsEntry_t*	ssentry = NULL;
	size_t			pathmem = 0;
	char*			path_full = NULL;		// path with a root dir of this processing
	size_t			pl = 0;
	size_t			nl = 0;

	pl = strlen( path );
	nl = strlen( name );

	// allocating memory for bbbSsEntry_t + path, pathmem will be aligned to BBB_WORD_SIZE
	// in order to get properly aligned memory after load_snapshot()
	pathmem = ( pl - skip + nl + 1 + BBB_WORD_SIZE ) & ~( BBB_WORD_SIZE - 1 );
	ssentry = malloc( sizeof( bbbSsEntry_t ) + pathmem );

	if ( ssentry == NULL ) {
		BBB_PERR( "Cannot allocate memory for an entry: %s\n", strerror( errno ) );
		return 0;
	}

	path_full = malloc( pathmem + skip + 1 );
	strncpy( path_full, path, pl + 1 );
	strncat( path_full, "/", 2 );
	strncat( path_full, name, nl + 1 );

	ssentry->status = 0;
	ssentry->custom = 0;
	ssentry->pathmem = pathmem;
	strncpy( BBB_SS_ENTRY_PATH( ssentry ), path_full + skip + 1, pathmem );

	if ( stat( path_full, &entry_info ) ) {
		BBB_PERR( "Cannot get info about %s: %s\n", path_full, strerror( errno ) );
		free( ssentry );
		free( path_full );
		return 0;
	}

	ssentry->content.size = entry_info.st_size;
	ssentry->content.mtime = entry_info.st_mtime;

	if ( S_ISDIR( entry_info.st_mode ) ) {
		ssentry->status |= BBB_SS_ENTRY_STATUS_DIR;
		_ProcessDir( path_full, skip, ss );
	} else if ( S_ISREG( entry_info.st_mode ) ) {
		ssentry->status &= ~BBB_SS_ENTRY_STATUS_DIR;
	} else {
		BBB_PLOG( "Skipping irregular file: %s\n", path_full );
		free( ssentry );
		free( path_full );
		return 1;		// it is a successful operation
	}

	free( path_full );
	return _AddToSnapshot( ssentry, ss );
}

static int _AddToSnapshot( bbbSsEntry_t* const ssentry, bbbSnapshot_t* const ss ) {
	bbbSsHash_t		hash;

	if ( ss->restored ) {
		BBB_PERR( "Adding entries to a restored snapshot is denied." );
		return 0;
	}

	hash = bbbHashBuf_uint16( BBB_SS_ENTRY_PATH( ssentry ), strlen( BBB_SS_ENTRY_PATH( ssentry ) ) );

	// push to the beginning of the list
	ssentry->next = ss->ht[ hash ].first;
	ss->ht[ hash ].first = ssentry;
	ss->ht[ hash ].size += sizeof( bbbSsEntry_t ) + ssentry->pathmem;

	return 1;
}

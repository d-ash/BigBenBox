#include "pack.h"

static void		_ConstructPfHeader( unsigned char pfh[ BBB_PF_HEADER_SIZE ] );
static int		_PackSnapshot( FILE* f, const bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p );
static int		_UnpackSnapshot( FILE* f, bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p );

int BbbSaveSnapshot( const char* const path, const bbbSnapshot_t* const ss ) {
	FILE* f;
	unsigned char pfh[ BBB_PF_HEADER_SIZE ];
	bbbPfHeaderExt_t pfh_ext;
	int res = 0;
	bbbChecksum_t checksum = 0;

	f = fopen( path, "wb" );
	if ( f == NULL ) {
		BBB_PERR( "Cannot write a snapshot to %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	_ConstructPfHeader( pfh );
	if ( fwrite( pfh, sizeof( pfh ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot write a header to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUpdateChecksum( pfh, sizeof( pfh ), &checksum );

	pfh_ext.tf_pathmem = strlen( ss->tf_path ) + 1;
	if ( fwrite( &pfh_ext, sizeof( pfh_ext ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot write an extended header to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUpdateChecksum( &pfh_ext, sizeof( pfh_ext ), &checksum );

	if ( fwrite( ss->tf_path, pfh_ext.tf_pathmem, 1, f ) < 1 ) {
		BBB_PERR( "Cannot write tf_path to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUpdateChecksum( ss->tf_path, pfh_ext.tf_pathmem, &checksum );

	res = _PackSnapshot( f, ss, &checksum );

	// checksum is saved in a network order
	checksum = htonl( checksum );
	if ( fwrite( &checksum, sizeof( checksum ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot write a checksum to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	if ( fclose( f ) != 0 ) {
		BBB_PERR( "Cannot save a snapshot to %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	if ( !res ) {
		unlink( path );
	}

	return res;
}

int BbbLoadSnapshot( const char* const path, bbbSnapshot_t* const ss ) {
	FILE* f = NULL;
	unsigned char pfh[ BBB_PF_HEADER_SIZE ];
	unsigned char pfh_control[ BBB_PF_HEADER_SIZE ];
	bbbPfHeaderExt_t pfh_ext;
	int res = 0;
	bbbChecksum_t checksum = 0;
	bbbChecksum_t checksum_read = 0;

	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	BbbInitSnapshot( ss );
	ss->restored = 1;

	f = fopen( path, "rb" );
	if ( f == NULL ) {
		BBB_PERR( "Cannot open %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fread( pfh, sizeof( pfh ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read from %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUpdateChecksum( pfh, sizeof( pfh ), &checksum );

	_ConstructPfHeader( pfh_control );
	if ( memcmp( pfh, pfh_control, sizeof( pfh ) ) != 0 ) {
		BBB_PERR( "Header of the snapshot file %s is incorrect.\n", path );
		BbbDestroySnapshot( ss );
		return 0;
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if ( fread( &pfh_ext, sizeof( pfh_ext ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read an extended header from a snapshot file: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUpdateChecksum( &pfh_ext, sizeof( pfh_ext ), &checksum );

	ss->tf_path = malloc( pfh_ext.tf_pathmem );
	if ( ss->tf_path == NULL ) {
		BBB_PERR( "Cannot allocate memory: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fread( ss->tf_path, pfh_ext.tf_pathmem, 1, f ) < 1 ) {
		BBB_PERR( "Cannot read tf_path from a snapshot file: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUpdateChecksum( ss->tf_path, pfh_ext.tf_pathmem, &checksum );

	res = _UnpackSnapshot( f, ss, &checksum );

	// checksum will be overread by the previous fread()
	if ( fseek( f, 0 - sizeof( checksum_read ), SEEK_END ) != 0 ) {
		BBB_PERR( "Cannot fseek() to a checksum of the file %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fread( &checksum_read, sizeof( checksum_read ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read a checksum from the snapshot file %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	// it was stored in a network order
	if ( htonl( checksum ) != checksum_read ) {
		BBB_PERR( "The snapshot file %s is corrupted (checksum failed): %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fclose( f ) != 0 ) {
		BBB_PERR( "Cannot close %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( !res ) {
		BbbDestroySnapshot( ss );
	}

	return res;
}

static void _ConstructPfHeader( unsigned char pfh[ BBB_PF_HEADER_SIZE ] ) {
	pfh[ 0 ] = BBB_PF_MAGIC;
	pfh[ 1 ] = ( BbbIsLittleEndian() ? 1 : 0 ) | BBB_WORD_SIZE;
	pfh[ 2 ] = BBB_PLATFORM_ID;
	pfh[ 3 ] = BBB_PF_STRUCT_VER;
}

static int _PackSnapshot( FILE* f, const bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p ) {
	bbbSsHash_t i;
	bbbSsEntry_t* ssentry = NULL;
	bbbSsHashHeader_t* cur_hh = NULL;
	bbbPfHashHeader_t h;

	for ( i = 0; i < BBB_SS_HASH_MAX; i++ ) {
		cur_hh = & ss->ht[ i ];

		if ( cur_hh->first == NULL ) {
			// Do not store hashes with no entries, it's packing anyway.
			continue;
		}

		h.hash = i;
		h.size = cur_hh->size;

		if ( fwrite( &h, sizeof( h ), 1, f ) < 1 ) {
			return 0;
		}
		BbbUpdateChecksum( &h, sizeof( h ), checksum_p );

		ssentry = cur_hh->first;
		do {
			if ( fwrite( ssentry, sizeof( bbbSsEntry_t ) + ssentry->pathmem, 1, f ) < 1 ) {
				return 0;
			}
			BbbUpdateChecksum( ssentry, sizeof( bbbSsEntry_t ) + ssentry->pathmem, checksum_p );

			ssentry = ssentry->next;
		} while ( ssentry != NULL );
	}

	return 1;
}

static int _UnpackSnapshot( FILE* f, bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p ) {
	bbbPfHashHeader_t h;
	bbbSsEntry_t* ssentry = NULL;
	bbbSsHashHeader_t* cur_hh = NULL;
	unsigned char* max_next = NULL;

	// iterating over the hash list
	while ( fread( &h, sizeof( h ), 1, f ) == 1 ) {
		BbbUpdateChecksum( &h, sizeof( h ), checksum_p );

		// allocating memory for all entries with this hash
		cur_hh = & ss->ht[ h.hash ];
		cur_hh->size = h.size;
		cur_hh->first = malloc( h.size );

		if ( cur_hh->first == NULL ) {
			BBB_PERR( "Cannot allocate memory for a entries list: %s\n", strerror( errno ) );
			return 0;
		}

		// The highest possible pointer value (counting not empty string).
		max_next = ( unsigned char* ) cur_hh->first + h.size - sizeof( bbbSsEntry_t ) - 2;

		if ( fread( cur_hh->first, h.size, 1, f ) < 1 ) {
			BBB_PERR( "Cannot read from a snapshot file: %s\n", strerror( errno ) );
			return 0;
		}
		BbbUpdateChecksum( cur_hh->first, h.size, checksum_p );

		// Set correct values for bbbSsEntry_t.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		ssentry = cur_hh->first;
		while ( ssentry->next != NULL ) {
			ssentry->next = ( unsigned char* ) ssentry + sizeof( bbbSsEntry_t ) + ssentry->pathmem;

			if ( ( unsigned char* ) ssentry->next > max_next ) {
				BBB_PERR( "Snapshot file is corrupted!\n" );
				return 0;
			}

			ssentry = ssentry->next;
		}
	}

	if ( !feof( f ) ) {
		BBB_PERR( "Cannot read from a snapshot file: %s\n", strerror( errno ) );
		return 0;
	}

	return 1;
}

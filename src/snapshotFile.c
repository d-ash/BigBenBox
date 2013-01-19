#include "snapshotFile.h"
#include "util.h"
#include "utilHash.h"

static void		_ConstructSsFileHdr( bbbByte_t hdr[ BBB_SS_FILE_HDR_SIZE ] );
static int		_PackSnapshot( FILE* const f, const bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p );
static int		_UnpackSnapshot( FILE* const f, bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p );

int BbbSaveSnapshot( const char* const path, const bbbSnapshot_t* const ss ) {
	FILE*				f;
	bbbByte_t			hdr[ BBB_SS_FILE_HDR_SIZE ];
	bbbSsFileHdrExt_t	hdrExt;
	int					res = 0;
	bbbChecksum_t		checksum = 0;

	f = fopen( path, "wb" );
	if ( f == NULL ) {
		BBB_PERR( "Cannot write a snapshot to %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	_ConstructSsFileHdr( hdr );
	if ( fwrite( hdr, sizeof( hdr ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot write a header to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUtilHashChecksum( hdr, sizeof( hdr ), &checksum );

	hdrExt.takenFromMem = strlen( ss->takenFrom ) + 1;
	if ( fwrite( &hdrExt, sizeof( hdrExt ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot write an extended header to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUtilHashChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	if ( fwrite( ss->takenFrom, hdrExt.takenFromMem, 1, f ) < 1 ) {
		BBB_PERR( "Cannot write 'takenFrom' to the snapshot file %s: %s\n", path, strerror( errno ) );
		return 0;
	}
	BbbUtilHashChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

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
	FILE*				f = NULL;
	bbbByte_t			hdr[ BBB_SS_FILE_HDR_SIZE ];
	bbbByte_t			hdrControl[ BBB_SS_FILE_HDR_SIZE ];
	bbbSsFileHdrExt_t	hdrExt;
	int					res = 0;
	bbbChecksum_t		checksum = 0;
	bbbChecksum_t		checksumRead = 0;

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

	if ( fread( hdr, sizeof( hdr ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read from %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUtilHashChecksum( hdr, sizeof( hdr ), &checksum );

	_ConstructSsFileHdr( hdrControl );
	if ( memcmp( hdr, hdrControl, sizeof( hdr ) ) != 0 ) {
		BBB_PERR( "Header of the snapshot file %s is incorrect.\n", path );
		BbbDestroySnapshot( ss );
		return 0;
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if ( fread( &hdrExt, sizeof( hdrExt ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read an extended header from a snapshot file: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUtilHashChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	ss->takenFrom = malloc( hdrExt.takenFromMem );
	if ( ss->takenFrom == NULL ) {
		BBB_PERR( "Cannot allocate memory: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fread( ss->takenFrom, hdrExt.takenFromMem, 1, f ) < 1 ) {
		BBB_PERR( "Cannot read 'takenFrom' from a snapshot file: %s\n", strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}
	BbbUtilHashChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	res = _UnpackSnapshot( f, ss, &checksum );

	// checksum will be overread by the previous fread()
	if ( fseek( f, 0 - sizeof( checksumRead ), SEEK_END ) != 0 ) {
		BBB_PERR( "Cannot fseek() to a checksum of the file %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	if ( fread( &checksumRead, sizeof( checksumRead ), 1, f ) < 1 ) {
		BBB_PERR( "Cannot read a checksum from the snapshot file %s: %s\n", path, strerror( errno ) );
		BbbDestroySnapshot( ss );
		return 0;
	}

	// it was stored in a network order
	if ( htonl( checksum ) != checksumRead ) {
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

static void _ConstructSsFileHdr( bbbByte_t hdr[ BBB_SS_FILE_HDR_SIZE ] ) {
	hdr[ 0 ] = BBB_SS_FILE_MAGIC;
	hdr[ 1 ] = ( BbbUtilIsLittleEndian() ? 1 : 0 ) | BBB_WORD_SIZE;
	hdr[ 2 ] = BBB_PLATFORM_ID;
	hdr[ 3 ] = BBB_SS_FILE_VERSION;
}

static int _PackSnapshot( FILE* const f, const bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p ) {
	bbbSsHash_t			i;
	bbbSsEntry_t*		entry = NULL;
	bbbSsHashHdr_t*		hashHdr = NULL;
	bbbSsFileHashHdr_t	fileHashHdr;

	for ( i = 0; i < BBB_SS_HASH_MAX; i++ ) {
		hashHdr = & ss->ht[ i ];

		if ( hashHdr->first == NULL ) {
			// Do not store hashes with no entries, it's packing anyway.
			continue;
		}

		fileHashHdr.hash = i;
		fileHashHdr.size = hashHdr->size;

		if ( fwrite( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) < 1 ) {
			return 0;
		}
		BbbUtilHashChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		entry = hashHdr->first;
		do {
			if ( fwrite( entry, sizeof( bbbSsEntry_t ) + entry->pathMem, 1, f ) < 1 ) {
				return 0;
			}
			BbbUtilHashChecksum( entry, sizeof( bbbSsEntry_t ) + entry->pathMem, checksum_p );

			entry = entry->next;
		} while ( entry != NULL );
	}

	return 1;
}

static int _UnpackSnapshot( FILE* const f, bbbSnapshot_t* const ss, bbbChecksum_t* checksum_p ) {
	bbbSsFileHashHdr_t	fileHashHdr;
	bbbSsEntry_t*		entry = NULL;
	bbbSsHashHdr_t*		hashHdr = NULL;
	bbbByte_t*			maxPtr = NULL;

	// iterating over the hash list
	while ( fread( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) == 1 ) {
		BbbUtilHashChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		// allocating memory for all entries with this hash
		hashHdr = & ss->ht[ fileHashHdr.hash ];
		hashHdr->size = fileHashHdr.size;
		hashHdr->first = malloc( fileHashHdr.size );

		if ( hashHdr->first == NULL ) {
			BBB_PERR( "Cannot allocate memory for a entries list: %s\n", strerror( errno ) );
			return 0;
		}

		// The highest possible pointer value (counting not empty string).
		maxPtr = ( bbbByte_t* ) hashHdr->first + fileHashHdr.size - sizeof( bbbSsEntry_t ) - 2;

		if ( fread( hashHdr->first, fileHashHdr.size, 1, f ) < 1 ) {
			BBB_PERR( "Cannot read from a snapshot file: %s\n", strerror( errno ) );
			return 0;
		}
		BbbUtilHashChecksum( hashHdr->first, fileHashHdr.size, checksum_p );

		// Set correct values for bbbSsEntry_t.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		entry = hashHdr->first;
		while ( entry->next != NULL ) {
			entry->next = ( bbbByte_t* ) entry + sizeof( bbbSsEntry_t ) + entry->pathMem;

			if ( ( bbbByte_t* ) entry->next > maxPtr ) {
				BBB_PERR( "Snapshot file is corrupted!\n" );
				return 0;
			}

			entry = entry->next;
		}
	}

	if ( !feof( f ) ) {
		BBB_PERR( "Cannot read from a snapshot file: %s\n", strerror( errno ) );
		return 0;
	}

	return 1;
}

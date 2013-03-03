<?:include c_lang.p ?>

#include "bbb_sshot_file.h"
#include "bbb_util.h"
#include "bbb_util_hash.h"

<?:prefix @_ bbb_sshot_file_ ?>
<?:prefix @^ BBB_SSHOT_FILE_ ?>

static void				_ConstructHdr( @_hdr_t* const hdr );
static bbb_result_t		_Pack( FILE* const f, const bbb_sshot_t* const ss, bbb_checksum_t* checksum_p );
static bbb_result_t		_Unpack( FILE* const f, bbb_sshot_t* const ss, bbb_checksum_t* checksum_p );

// ============================================

bbb_result_t
@_Save( const char* const path, const bbb_sshot_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;
	FILE*			f;
	@_hdr_t			hdr;
	@_hdr2_t		hdrExt;
	bbb_checksum_t	checksum = 0;

	f = fopen( path, "wb" );
	if ( f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		if ( fclose( f ) != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
		if ( BBB_FAILED( result ) ) {
			unlink( path );
		}
	<?" ); ?>

	_ConstructHdr( &hdr );
	<? c_OnCleanup( "?>
		@_Destroy_hdr( &hdr );
	<?" ); ?>

	if ( BBB_FAILED( result = @_WriteToFile_hdr( &hdr, f, &checksum ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	hdrExt.takenFromMem = strlen( ss->takenFrom ) + 1;
	if ( BBB_FAILED( result = bbb_util_Fwrite( &hdrExt, sizeof( hdrExt ), 1, f ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	if ( BBB_FAILED( result = bbb_util_Fwrite( ss->takenFrom, hdrExt.takenFromMem, 1, f ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	if ( BBB_FAILED( result = _Pack( f, ss, &checksum ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( BBB_FAILED( result = bbb_bio_WriteToFile_uint32( checksum, f, NULL ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_Load( const char* const path, bbb_sshot_t* const ss ) {
	bbb_result_t	result = BBB_SUCCESS;
	FILE*			f = NULL;
	@_hdr_t			hdr;
	@_hdr_t			hdrControl;
	@_hdr2_t		hdrExt;
	bbb_checksum_t	checksum = 0;
	bbb_checksum_t	checksumRead = 0;
	size_t			wasRead;

	bbb_sshot_Init( ss );
	<? c_OnCleanup( "?>
		if ( BBB_FAILED( result ) ) {
			bbb_sshot_Destroy( ss );
		}
	<?" ); ?>
	ss->restored = 1;

	f = fopen( path, "rb" );
	if ( f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		if ( fclose( f ) != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
	<?" ); ?>

	if ( BBB_FAILED( result = @_ReadFromFile_hdr( &hdr, f, &checksum ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		@_Destroy_hdr( &hdr );
	<?" ); ?>

	_ConstructHdr( &hdrControl );
	<? c_OnCleanup( "?>
		@_Destroy_hdr( &hdrControl );
	<?" ); ?>

	if ( !@_IsEqual_hdr( &hdr, &hdrControl ) ) {
		BBB_ERR_CODE( BBB_ERROR_INVALIDSSHOT, "The snapshot %s was saved on another machine", path );
		result = BBB_ERROR_INVALIDSSHOT;
		<? c_GotoCleanup(); ?>
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if ( BBB_FAILED( result = bbb_util_Fread( &hdrExt, sizeof( hdrExt ), 1, f, &wasRead ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	if ( wasRead == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "Cannot read an 'hdrExt' from %s", path );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	// takenFrom will be released in bbb_sshot_Destroy() if is not NULL
	if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&( ss->takenFrom ), hdrExt.takenFromMem ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( BBB_FAILED( result = bbb_util_Fread( ss->takenFrom, hdrExt.takenFromMem, 1, f, &wasRead ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	if ( wasRead == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "Cannot read 'takenFrom' from %s", path );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}

	bbb_util_hash_UpdateChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	if ( BBB_FAILED( result = _Unpack( f, ss, &checksum ) ) ) {
		if ( result == BBB_ERROR_CORRUPTEDDATA ) {
			BBB_ERR_CODE( BBB_ERROR_INVALIDSSHOT, "Contents of the snapshot file %s cannot be unpacked", path );
			result = BBB_ERROR_INVALIDSSHOT;
		}
		<? c_GotoCleanup(); ?>
	}

	// checksum will be read over by the previous fread()
	if ( fseek( f, 0 - sizeof( checksumRead ), SEEK_END ) != 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}

	if ( BBB_FAILED( result = bbb_bio_ReadFromFile_uint32( &checksumRead, f, NULL ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( checksum != checksumRead ) {
		BBB_ERR_CODE( BBB_ERROR_INVALIDSSHOT, "Incorrect checksum of the snapshot file %s", path );
		result = BBB_ERROR_INVALIDSSHOT;
		<? c_GotoCleanup(); ?>
	}

	<? c_Cleanup(); ?>
	return result;
}

static void
_ConstructHdr( @_hdr_t* const hdr ) {
	hdr->magic		= @^MAGIC;
	hdr->runtime	= ( bbb_util_IsLittleEndian() ? 1 : 0 ) | BBB_WORD_SIZE;
	hdr->platform	= BBB_PLATFORM_ID;
	hdr->format		= @^FORMAT;
}

static bbb_result_t
_Pack( FILE* const f, const bbb_sshot_t* const ss, bbb_checksum_t* checksum_p ) {
	bbb_result_t		result = BBB_SUCCESS;
	bbb_sshot_hash_t	i;
	bbb_sshot_entry_t*	entry = NULL;
	bbb_sshot_ht_t*		hashHdr = NULL;
	@_ht_t				fileHashHdr;

	for ( i = 0; i < BBB_SSHOT_HASH_MAX; i++ ) {
		hashHdr = & ss->ht[ i ];

		if ( hashHdr->first == NULL ) {
			// Do not store hashes with no entries, it's packing.
			continue;
		}

		fileHashHdr.hash = i;
		fileHashHdr.size = hashHdr->size;

		if ( BBB_FAILED( result = bbb_util_Fwrite( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) ) ) {
			goto L_end;
		}
		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		entry = hashHdr->first;
		do {
			if ( BBB_FAILED( result = bbb_util_Fwrite( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, 1, f ) ) ) {
				goto L_end;
			}
			bbb_util_hash_UpdateChecksum( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, checksum_p );

			entry = entry->next;
		} while ( entry != NULL );
	}

L_end:
	return result;
}

static bbb_result_t
_Unpack( FILE* const f, bbb_sshot_t* const ss, bbb_checksum_t* checksum_p ) {
	bbb_result_t		result = BBB_SUCCESS;
	@_ht_t				fileHashHdr;
	bbb_sshot_entry_t*	entry = NULL;
	bbb_sshot_ht_t*		hashHdr = NULL;
	bbb_byte_t*			maxPtr = NULL;
	size_t				wasRead;

	// iterating over the hash list
	do {
		if ( BBB_FAILED( result = bbb_util_Fread( &fileHashHdr, sizeof( fileHashHdr ), 1, f, &wasRead ) ) ) {
			goto L_end;
		}
		if ( wasRead == 0 ) {
			break;
		}

		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		// Allocating memory for all entries with this hash.
		// In case of error this will be released at snapshot's destruction.
		hashHdr = &( ss->ht[ fileHashHdr.hash ] );
		hashHdr->size = fileHashHdr.size;
		if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&( hashHdr->first ), fileHashHdr.size ) ) ) {
			goto L_end;
		}

		if ( BBB_FAILED( result = bbb_util_Fread( hashHdr->first, fileHashHdr.size, 1, f, &wasRead ) ) ) {
			goto L_end;
		}
		if ( wasRead == 0 ) {
			BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "Unexpected end of the snapshot file (entities)" );
			result = BBB_ERROR_CORRUPTEDDATA;
			goto L_end;
		}
		bbb_util_hash_UpdateChecksum( hashHdr->first, fileHashHdr.size, checksum_p );

		// The highest possible pointer value (counting a non-empty string).
		maxPtr = ( bbb_byte_t* ) hashHdr->first + fileHashHdr.size - sizeof( bbb_sshot_entry_t ) - 2;

		// Set correct values for bbb_sshot_entry_t.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		entry = hashHdr->first;
		while ( entry->next != NULL ) {
			entry->next = ( bbb_byte_t* ) entry + sizeof( bbb_sshot_entry_t ) + entry->pathMem;

			if ( ( bbb_byte_t* ) entry->next > maxPtr ) {
				BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "The snapshot file is damaged (maxPtr)" );
				result = BBB_ERROR_CORRUPTEDDATA;
				goto L_end;
			}

			entry = entry->next;
		}
	} while ( 1 );

	if ( ferror( f ) ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "%s", strerror( errno ) );
		result = BBB_ERROR_CORRUPTEDDATA;
		goto L_end;
	}

L_end:
	return result;
}

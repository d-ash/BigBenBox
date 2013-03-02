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
	size_t			dummy;

	f = fopen( path, "wb" );
	if ( f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "f", "?>
		if ( fclose( f ) != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
		if ( BBB_FAILED( result ) ) {
			unlink( path );
		}
	<?"); ?>

	_ConstructHdr( &hdr );
	<? c_OnCleanup( "hdr", "?>
		@_Destroy_hdr( &hdr );
	<?"); ?>

	if ( @_WriteToFile_hdr( &hdr, f, &checksum ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}

	hdrExt.takenFromMem = strlen( ss->takenFrom ) + 1;
	if ( fwrite( &hdrExt, sizeof( hdrExt ), 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	if ( fwrite( ss->takenFrom, hdrExt.takenFromMem, 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	if ( BBB_FAILED( result = _Pack( f, ss, &checksum ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( BBB_FAILED( result = bbb_bio_WriteToFile_uint32( checksum, f, NULL, &dummy ) ) ) {
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
	size_t			dummy;

	bbb_sshot_Init( ss );
	<? c_OnCleanup( "ss", "?>
		if ( BBB_FAILED( result ) ) {
			bbb_sshot_Destroy( ss );
		}
	<?"); ?>
	ss->restored = 1;

	f = fopen( path, "rb" );
	if ( f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "f", "?>
		if ( fclose( f ) != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
	<?"); ?>

	if ( @_ReadFromFile_hdr( &hdr, f, &checksum ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "hdr", "?>
		@_Destroy_hdr( &hdr );
	<?"); ?>

	_ConstructHdr( &hdrControl );
	<? c_OnCleanup( "hdrControl", "?>
		@_Destroy_hdr( &hdrControl );
	<?"); ?>

	if ( !@_IsEqual_hdr( &hdr, &hdrControl ) ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "Header of the snapshot file %s is incorrect", path );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if ( fread( &hdrExt, sizeof( hdrExt ), 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	// takenFrom will be released in bbb_sshot_Destroy() if is not NULL
	if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( ss->takenFrom ), hdrExt.takenFromMem ) ) ) {
		result = BBB_ERROR_NOMEMORY;
		<? c_GotoCleanup(); ?>
	}

	if ( fread( ss->takenFrom, hdrExt.takenFromMem, 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
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

	if ( BBB_FAILED( result = bbb_bio_ReadFromFile_uint32( &checksumRead, f, NULL, &dummy ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( checksum != checksumRead ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
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

		if ( fwrite( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) == 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			return BBB_ERROR_FILESYSTEMIO;
		}
		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		entry = hashHdr->first;
		do {
			if ( fwrite( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, 1, f ) == 0 ) {
				BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
				return BBB_ERROR_FILESYSTEMIO;
			}
			bbb_util_hash_UpdateChecksum( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, checksum_p );

			entry = entry->next;
		} while ( entry != NULL );
	}

	return BBB_SUCCESS;
}

static bbb_result_t
_Unpack( FILE* const f, bbb_sshot_t* const ss, bbb_checksum_t* checksum_p ) {
	@_ht_t				fileHashHdr;
	bbb_sshot_entry_t*	entry = NULL;
	bbb_sshot_ht_t*		hashHdr = NULL;
	bbb_byte_t*			maxPtr = NULL;

	// iterating over the hash list
	while ( fread( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) == 1 ) {
		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		// Allocating memory for all entries with this hash.
		// In case of error this will be released at snapshot's Destroy().
		hashHdr = &( ss->ht[ fileHashHdr.hash ] );
		hashHdr->size = fileHashHdr.size;
		if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( hashHdr->first ), fileHashHdr.size ) ) ) {
			return BBB_ERROR_NOMEMORY;
		}

		// The highest possible pointer value (counting not empty string).
		maxPtr = ( bbb_byte_t* ) hashHdr->first + fileHashHdr.size - sizeof( bbb_sshot_entry_t ) - 2;

		if ( fread( hashHdr->first, fileHashHdr.size, 1, f ) == 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			return BBB_ERROR_FILESYSTEMIO;
		}
		bbb_util_hash_UpdateChecksum( hashHdr->first, fileHashHdr.size, checksum_p );

		// Set correct values for bbb_sshot_entry_t.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		entry = hashHdr->first;
		while ( entry->next != NULL ) {
			entry->next = ( bbb_byte_t* ) entry + sizeof( bbb_sshot_entry_t ) + entry->pathMem;

			if ( ( bbb_byte_t* ) entry->next > maxPtr ) {
				BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA, "Snapshot file is damaged!" );
				return BBB_ERROR_CORRUPTEDDATA;
			}

			entry = entry->next;
		}
	}

	if ( !feof( f ) ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}

	return BBB_SUCCESS;
}

#include "sshot_file.h"
#include "util.h"
#include "util_hash.h"
#include "bio.h"

<?:prefix @_ bbb_sshot_file_ ?>
<?:prefix @^ BBB_SSHOT_FILE_ ?>

<? my $cleanup; ?>
<? my $cleanup2; ?>

static void		_ConstructHdr( @_hdr_t* const hdr );
static int		_Pack( FILE* const f, const bbb_sshot_t* const ss, bbb_checksum_t* checksum_p );
static int		_Unpack( FILE* const f, bbb_sshot_t* const ss, bbb_checksum_t* checksum_p );

// ============================================

int @_Save( const char* const path, const bbb_sshot_t* const ss ) {
	FILE*			f;
	@_hdr_t			hdr;
	@_hdr2_t		hdrExt;
	int				res = 0;
	bbb_checksum_t	checksum = 0;

	f = fopen( path, "wb" );
	if ( f == NULL ) {
		BBB_PERR( "Cannot write a snapshot to %s: %s\n", path, strerror( errno ) );
		return 0;
	}

	_ConstructHdr( &hdr );
	<? $cleanup = "?>@_Destroy_hdr( &hdr );<?"; ?>
	if ( @_WriteToFile_hdr( &hdr, f, &checksum ) == 0 ) {
		BBB_PERR( "Cannot write a header to the snapshot file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}

	hdrExt.takenFromMem = strlen( ss->takenFrom ) + 1;
	if ( fwrite( &hdrExt, sizeof( hdrExt ), 1, f ) == 0 ) {
		BBB_PERR( "Cannot write an extended header to the snapshot file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	if ( fwrite( ss->takenFrom, hdrExt.takenFromMem, 1, f ) == 0 ) {
		BBB_PERR( "Cannot write 'takenFrom' to the snapshot file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}
	bbb_util_hash_UpdateChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	res = _Pack( f, ss, &checksum );

	if ( bbb_bio_WriteToFile_uint32( checksum, f, NULL ) == 0 ) {
		BBB_PERR( "Cannot write a checksum to the snapshot file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}

	if ( fclose( f ) != 0 ) {
		BBB_PERR( "Cannot save a snapshot to %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}

	if ( !res ) {
		unlink( path );
	}

	<?= $cleanup ?>
	return res;
}

int @_Load( const char* const path, bbb_sshot_t* const ss ) {
	FILE*			f = NULL;
	@_hdr_t			hdr;
	@_hdr_t			hdrControl;
	@_hdr2_t		hdrExt;
	int				res = 0;
	bbb_checksum_t	checksum = 0;
	bbb_checksum_t	checksumRead = 0;

	if ( ss == NULL ) {
		BBB_PERR( "NULL value in %s()\n", __FUNCTION__ );
		return 0;
	}

	bbb_sshot_Init( ss );
	<? $cleanup = "?>bbb_sshot_Destroy( ss );<?"; ?>
	ss->restored = 1;

	f = fopen( path, "rb" );
	if ( f == NULL ) {
		BBB_PERR( "Cannot open %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}

	if ( @_ReadFromFile_hdr( &hdr, f, &checksum ) == 0 ) {
		BBB_PERR( "Cannot read from %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		return 0;
	}
	<? $cleanup2 = "?>@_Destroy_hdr( &hdr );<?"; ?>

	_ConstructHdr( &hdrControl );
	<? $cleanup2 .= "?>@_Destroy_hdr( &hdrControl );<?"; ?>
	if ( !@_IsEqual_hdr( &hdr, &hdrControl ) ) {
		BBB_PERR( "Header of the snapshot file %s is incorrect.\n", path );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}

	// Reading extended header. Platform dependent types are already in use!
	// We can correctly read files only created with this same program on this machine.
	if ( fread( &hdrExt, sizeof( hdrExt ), 1, f ) == 0 ) {
		BBB_PERR( "Cannot read an extended header from a snapshot file: %s\n", strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}
	bbb_util_hash_UpdateChecksum( &hdrExt, sizeof( hdrExt ), &checksum );

	ss->takenFrom = BBB_UTIL_MALLOC( hdrExt.takenFromMem );

	if ( fread( ss->takenFrom, hdrExt.takenFromMem, 1, f ) == 0 ) {
		BBB_PERR( "Cannot read 'takenFrom' from a snapshot file: %s\n", strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}
	bbb_util_hash_UpdateChecksum( ss->takenFrom, hdrExt.takenFromMem, &checksum );

	res = _Unpack( f, ss, &checksum );

	// checksum will be overread by the previous fread()
	if ( fseek( f, 0 - sizeof( checksumRead ), SEEK_END ) != 0 ) {
		BBB_PERR( "Cannot fseek() to a checksum of the file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}

	if ( bbb_bio_ReadFromFile_uint32( &checksumRead, f, NULL ) == 0 ) {
		BBB_PERR( "Cannot read a checksum from the snapshot file %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}

	if ( checksum != checksumRead ) {
		BBB_PERR( "The snapshot file %s is corrupted (checksum failed): %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}

	if ( fclose( f ) != 0 ) {
		BBB_PERR( "Cannot close %s: %s\n", path, strerror( errno ) );
		<?= $cleanup ?>
		<?= $cleanup2 ?>
		return 0;
	}

	if ( !res ) {
		<?= $cleanup ?>
	}

	<?= $cleanup2 ?>
	return res;
}

static void _ConstructHdr( @_hdr_t* const hdr ) {
	hdr->magic		= @^MAGIC;
	hdr->runtime	= ( bbb_util_IsLittleEndian() ? 1 : 0 ) | BBB_WORD_SIZE;
	hdr->platform	= BBB_PLATFORM_ID;
	hdr->format		= @^FORMAT;
}

static int _Pack( FILE* const f, const bbb_sshot_t* const ss, bbb_checksum_t* checksum_p ) {
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
			return 0;
		}
		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		entry = hashHdr->first;
		do {
			if ( fwrite( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, 1, f ) == 0 ) {
				return 0;
			}
			bbb_util_hash_UpdateChecksum( entry, sizeof( bbb_sshot_entry_t ) + entry->pathMem, checksum_p );

			entry = entry->next;
		} while ( entry != NULL );
	}

	return 1;
}

static int _Unpack( FILE* const f, bbb_sshot_t* const ss, bbb_checksum_t* checksum_p ) {
	@_ht_t				fileHashHdr;
	bbb_sshot_entry_t*	entry = NULL;
	bbb_sshot_ht_t*		hashHdr = NULL;
	bbb_byte_t*			maxPtr = NULL;

	// iterating over the hash list
	while ( fread( &fileHashHdr, sizeof( fileHashHdr ), 1, f ) == 1 ) {
		bbb_util_hash_UpdateChecksum( &fileHashHdr, sizeof( fileHashHdr ), checksum_p );

		// allocating memory for all entries with this hash
		hashHdr = & ss->ht[ fileHashHdr.hash ];
		hashHdr->size = fileHashHdr.size;
		hashHdr->first = BBB_UTIL_MALLOC( fileHashHdr.size );

		// The highest possible pointer value (counting not empty string).
		maxPtr = ( bbb_byte_t* ) hashHdr->first + fileHashHdr.size - sizeof( bbb_sshot_entry_t ) - 2;

		if ( fread( hashHdr->first, fileHashHdr.size, 1, f ) == 0 ) {
			BBB_PERR( "Cannot read from a snapshot file: %s\n", strerror( errno ) );
			return 0;
		}
		bbb_util_hash_UpdateChecksum( hashHdr->first, fileHashHdr.size, checksum_p );

		// Set correct values for bbb_sshot_entry_t.next, all restored pointers are incorrect,
		// but we are searching for NULL value!
		entry = hashHdr->first;
		while ( entry->next != NULL ) {
			entry->next = ( bbb_byte_t* ) entry + sizeof( bbb_sshot_entry_t ) + entry->pathMem;

			if ( ( bbb_byte_t* ) entry->next > maxPtr ) {
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

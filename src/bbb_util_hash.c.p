<?:include c_lang.p ?>

#include "bbb_util_hash.h"
#include "bbb_util.h"

<?:prefix @_ bbb_util_hash_ ?>
<?:prefix @^ BBB_UTIL_HASH_ ?>

uint32_t
@_Calc_uint32( const void* const buf, const size_t len ) {
	uint32_t	hash = 0;
	size_t		i;

	for ( i = 0; i < len; i++ ) {
		hash = ( uint32_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

uint16_t
@_Calc_uint16( const void* const buf, const size_t len ) {
	uint16_t	hash = 0;
	size_t		i;

	for ( i = 0; i < len; i++ ) {
		hash = ( uint16_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) - hash;
	}

	return hash;
}

void
@_UpdateChecksum( const void* const buf, const size_t len, bbb_checksum_t* const checksum ) {
	uint32_t	hash;
	size_t		i;

	hash = ( *checksum );

	for ( i = 0; i < len; i++ ) {
		hash = ( uint32_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	( *checksum ) = hash;
}

bbb_result_t
@_ReadFile_sha256( const char* const path, bbb_byte_t hash[ SHA256_DIGEST_LENGTH ] ) {
	bbb_result_t		result = BBB_SUCCESS;
	FILE*				f;
	SHA256_CTX			sha;
	char*				buf = NULL;
	static const size_t len = 32768;
	size_t				wasRead;
	
	f = fopen( path, "rb" );
	if ( f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "Cannot open the file %s: %s", path, strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		if ( fclose( f ) != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
			result = BBB_ERROR_FILESYSTEMIO;
		}
	<?" ); ?>

	if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&buf, len ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		free( buf );
	<?" ); ?>

	SHA256_Init( &sha );
	<? c_OnCleanup( "?>
		SHA256_Final( hash, &sha );
	<?" ); ?>

	do {
		if ( BBB_FAILED( result = bbb_util_Fread( buf, 1, len, f, &wasRead ) ) ) {
			break;
		}
		SHA256_Update( &sha, buf, wasRead );
	} while ( wasRead > 0 );

	<? c_Cleanup(); ?>
	return result;
}

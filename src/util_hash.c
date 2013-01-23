#include "util_hash.h"

uint32_t bbb_util_hash_Calc_uint32( const void* const buf, const size_t len ) {
	uint32_t	hash = 0;
	size_t		i;

	for ( i = 0; i < len; i++ ) {
		hash = ( uint32_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

uint16_t bbb_util_hash_Calc_uint16( const void* const buf, const size_t len ) {
	uint16_t	hash = 0;
	size_t		i;

	for ( i = 0; i < len; i++ ) {
		hash = ( uint16_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) - hash;
	}

	return hash;
}

void bbb_util_hash_UpdateChecksum( const void* const buf, const size_t len, bbb_checksum_t* const checksum ) {
	uint32_t	hash;
	size_t		i;

	hash = ( *checksum );

	for ( i = 0; i < len; i++ ) {
		hash = ( uint32_t ) ( ( bbb_byte_t* ) buf )[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	( *checksum ) = hash;
}

int bbb_util_hash_ReadFile_sha256( const char* const path, bbb_byte_t const hash[ SHA256_DIGEST_LENGTH ] ) {
    FILE*				f;
    SHA256_CTX			sha;
	char*				buf = NULL;
    static const size_t len = 32768;
	size_t				wasRead = 0;
	
	f = fopen( path, "rb" );
    if ( f == NULL ) {
		BBB_PERR( "Cannot open file %s in %s: %s\n", path, __FUNCTION__, strerror( errno ) );
		return 0;
	}

    SHA256_Init( &sha );

    buf = malloc( len );
    if ( buf == NULL ) {
		BBB_PERR( "Cannot allocate memory for SHA256 buffer: %s\n", strerror( errno ) );
		fclose( f );
		return 0;
	}

    while ( ( wasRead = fread( buf, 1, len, f ) ) ) {
        SHA256_Update( &sha, buf, wasRead );
    }

    SHA256_Final( hash, &sha );

	if ( ferror( f ) ) {
		BBB_PERR( "Cannot read the file %s: %s\n", path, strerror( errno ) );
		free( buf );
		fclose( f );
		return 0;
	}

    free( buf );
    fclose( f );
    return 1;
}

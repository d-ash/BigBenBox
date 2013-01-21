#include "util_bio.h"
#include "util.h"

size_t bbb_util_bio_Write_uint16( const uint16_t v, FILE* const f ) {
	uint16_t	t;

	t = htons( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_uint32( const uint32_t v, FILE* const f ) {
	uint32_t	t;

	t = htonl( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_uint64( const uint64_t v, FILE* const f ) {
	uint64_t	t;

	t = bbb_util_ConvertBinary_hton64( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_varbuf( const bbb_byte_t* const buf, size_t const len, FILE* const f ) {
	uint32_t	len32;

	len32 = ( uint32_t ) len;

	if ( bbb_util_bio_Write_uint32( len32, f ) == 0 ) {
		return 0;
	}

	return fwrite( buf, len, 1, f );
}

size_t bbb_util_bio_Read_uint16( uint16_t* v, FILE* const f ) {
	uint16_t	t;
	size_t		res;

	res = fread( &t, sizeof( t ), 1, f );

	if ( res == 1 ) {
		*v = ntohs( t );
	}

	return res;
}

size_t bbb_util_bio_Read_uint32( uint32_t* v, FILE* const f ) {
	uint32_t	t;
	size_t		res;

	res = fread( &t, sizeof( t ), 1, f );

	if ( res == 1 ) {
		*v = ntohl( t );
	}

	return res;
}

size_t bbb_util_bio_Read_uint64( uint64_t* v, FILE* const f ) {
	uint64_t	t;
	size_t		res;

	res = fread( &t, sizeof( t ), 1, f );

	if ( res == 1 ) {
		*v = bbb_util_ConvertBinary_ntoh64( t );
	}

	return res;
}

size_t bbb_util_bio_Read_varbuf( bbb_byte_t** buf, size_t* const len, FILE* const f ) {
	uint32_t	len32;

	if ( bbb_util_bio_Read_uint32( &len32, f ) == 0 ) {
		return 0;
	}

	*len = ( size_t ) len32;
	*buf = malloc( *len );

	if ( *buf == NULL ) {
		return 0;
	}

	return fread( *buf, *len, 1, f );
}

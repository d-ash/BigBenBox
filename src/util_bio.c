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

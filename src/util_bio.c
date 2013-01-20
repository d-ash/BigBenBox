#include "util_bio.h"

size_t bbb_util_bio_Write_uint8( const uint8_t v, FILE* const f ) {
	uint8_t		t;

	t = htonl( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_uint16( const uint16_t v, FILE* const f ) {
	uint16_t	t;

	t = htonl( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_uint32( const uint32_t v, FILE* const f ) {
	uint32_t	t;

	t = htonl( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_uint64( const uint64_t v, FILE* const f ) {
	uint64_t	t;

	t = htonl( v );
	return fwrite( &t, sizeof ( t ), 1, f );
}

size_t bbb_util_bio_Write_buf( const char* const buf, const size_t len, FILE* const f ) {
	return fwrite( buf, len, 1, f );
}

int bbb_util_bio_Read_uint8( uint8_t* v, FILE* const f ) {
}

int bbb_util_bio_Read_uint16( uint16_t* v, FILE* const f ) {
}

int bbb_util_bio_Read_uint32( uint32_t* v, FILE* const f ) {
}

int bbb_util_bio_Read_uint64( uint64_t* v, FILE* const f ) {
}

int bbb_util_bio_Read_buf( char* const buf, const size_t len, FILE* const f ) {
}

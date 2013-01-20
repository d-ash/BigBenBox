#include "util_bio.h"

/*
// converts an arbitrary large integer (preferrably >=64 bits) from big endian to host machine endian
template<typename T> static inline T bigen2host(const T& x)
{
	static const int one = 1;
	static const char sig = *(char*)&one; 

	if (sig == 0) return x; // for big endian machine just return the input

	T ret;
	int size = sizeof(T);
	char* src = (char*)&x + sizeof(T) - 1;
	char* dst = (char*)&ret;

	while (size-- > 0) *dst++ = *src--;

	return ret;
}
*/

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
}

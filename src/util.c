#include "util.h"

int bbb_util_IsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

uint64_t bbb_util_ConvertBinary_hton64( const uint64_t x ) {
	uint64_t	res = 1;
	char*		src;
	char*		dst;
	size_t		sz;

	if ( ( * ( char* ) &res ) == 0 ) {
		return x;	// host is big-endian
	}

	src = ( char* ) &x + sizeof( x ) - 1;
	dst = ( char* ) &res;

	for ( sz = sizeof( x ); sz > 0; sz-- ) {
		*dst++ = *src--;
	}

	return res;
}

uint64_t bbb_util_ConvertBinary_ntoh64( const uint64_t x ) {
	return bbb_util_ConvertBinary_hton64( x );
}

void bbb_util_PrintHex( const void* const b, const size_t len ) {
    size_t	i;

    for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( bbb_byte_t* ) b )[ i ] );
    }
}

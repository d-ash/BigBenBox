#include "util.h"

int bbb_util_IsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

void bbb_util_PrintHex( const void* const b, const size_t len ) {
    size_t	i;

    for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( bbb_byte_t* ) b )[ i ] );
    }
}

#include "util.h"

int BbbIsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

void BbbPrintHex( const void* const b, const size_t len ) {
    size_t	i;

    for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( byte_t* ) b )[ i ] );
    }
}

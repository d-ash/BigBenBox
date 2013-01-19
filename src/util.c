#include "util.h"

int BbbUtilIsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

void BbbUtilPrintHex( const void* const b, const size_t len ) {
    size_t	i;

    for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( bbbByte_t* ) b )[ i ] );
    }
}

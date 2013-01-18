#include <stdio.h>

#include "utils.h"

const int bbbIsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

void bbbPrintHex( const void* const b, const size_t len ) {
    size_t		i;

    for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( unsigned char* ) b )[ i ] );
    }
}

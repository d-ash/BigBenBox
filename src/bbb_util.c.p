#include "bbb_util.h"

<?:prefix @_ bbb_util_ ?>
<?:prefix @^ BBB_UTIL_ ?>

bbb_result_t @_Malloc( void** const ptr, const size_t size ) {
	*ptr = malloc( size );

	if ( *ptr == NULL ) {
		BBB_ERR( "Cannot allocate %" PRIuPTR " bytes in heap", size );
		return BBB_ERROR_NOMEMORY;
	}
	return BBB_SUCCESS;
}

int @_IsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

uint64_t @_ConvertBinary_hton64( const uint64_t x ) {
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

uint64_t @_ConvertBinary_ntoh64( const uint64_t x ) {
	return @_ConvertBinary_hton64( x );
}

void @_PrintHex( const void* const b, const size_t len ) {
	size_t	i;

	for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( bbb_byte_t* ) b )[ i ] );
	}
}

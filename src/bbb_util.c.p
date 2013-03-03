<?:include bbb.p ?>
<?:prefix @_ bbb_util_ ?>
<?:prefix @^ BBB_UTIL_ ?>

#include "bbb_util.h"

bbb_result_t
@_Malloc( void** const ptr, const size_t size ) {
	*ptr = malloc( size );
	if ( *ptr == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_NOMEMORY, "%" PRIuPTR " bytes", size );
		return BBB_ERROR_NOMEMORY;
	}
	return BBB_SUCCESS;
}

bbb_result_t
@_Strdup( const char* src, char** const dst ) {
	*dst = strdup( src );
	if ( *dst == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_NOMEMORY );
		return BBB_ERROR_NOMEMORY;
	}
	return BBB_SUCCESS;
}

bbb_result_t
@_Fopen( const char* const path, const char* const mode, FILE** const f ) {
	*f = fopen( path, mode );
	if ( *f == NULL ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "path: %s, mode: %s, %s", path, mode, strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
@_Fclose( FILE* f ) {
	if ( fclose( f ) != 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
@_Fread( void* const ptr, const size_t size, const size_t nmemb, FILE* stream, size_t* const nmembRead ) {
	*nmembRead = fread( ptr, size, nmemb, stream );
	if ( *nmembRead == 0 && ferror( stream ) ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
@_Fwrite( const void* const ptr, const size_t size, const size_t nmemb, FILE* stream ) {
	size_t	nmembWritten;

	nmembWritten = fwrite( ptr, size, nmemb, stream );
	if ( nmembWritten != nmemb ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

int
@_IsLittleEndian() {
	static const int	i = 1;

	return ( * ( char* ) &i == 1 );
}

uint64_t
@_ConvertBinary_hton64( const uint64_t x ) {
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

uint64_t
@_ConvertBinary_ntoh64( const uint64_t x ) {
	return @_ConvertBinary_hton64( x );
}

void
@_PrintHex( const void* const b, const size_t len ) {
	size_t	i;

	for ( i = 0; i < len; i++ ) {
		printf( "%02x", ( ( bbb_byte_t* ) b )[ i ] );
	}
}

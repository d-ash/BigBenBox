<?:prefix @_ bbb_util_ ?>
<?:prefix @^ BBB_UTIL_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "bbb.h"

// ================ Exported functions =============

bbb_result_t	@_Malloc( void** const ptr, const size_t size );
bbb_result_t	@_Fread( void* const ptr, const size_t size, const size_t nmemb, FILE* stream, size_t* const read );
bbb_result_t	@_Fwrite( const void* const ptr, const size_t size, const size_t nmemb, FILE* stream, size_t* const written );

int			@_IsLittleEndian();
uint64_t	@_ConvertBinary_hton64( const uint64_t x );
uint64_t	@_ConvertBinary_ntoh64( const uint64_t x );
void		@_PrintHex( const void* const b, const size_t len );

#endif

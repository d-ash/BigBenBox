<?:include	bbb.p ?>
<?:prefix	@_ bbb_util_ ?>
<?:prefix	@^ BBB_UTIL_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "bbb.h"

// ================ Exported functions =============

bbb_result_t	@_Malloc( void** const ptr, const size_t size );
bbb_result_t	@_Strdup( const char* src, char** const dst );
bbb_result_t	@_Fopen( const char* const path, const char* const mode, FILE** const f );
bbb_result_t	@_Fclose( FILE* f );

				// Error occurs if there was ferror().
bbb_result_t	@_Fread( void* const ptr, const size_t size, const size_t nmemb, FILE* stream, size_t* const nmembRead );

				// Error occurs if less than 'nmemb' members were written.
bbb_result_t	@_Fwrite( const void* const ptr, const size_t size, const size_t nmemb, FILE* stream );

int				@_IsLittleEndian();
uint64_t		@_ConvertBinary_hton64( const uint64_t x );
uint64_t		@_ConvertBinary_ntoh64( const uint64_t x );
void			@_PrintHex( const void* const b, const size_t len );

#endif

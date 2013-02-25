<?:prefix @_ bbb_util_ ?>
<?:prefix @^ BBB_UTIL_ ?>

#ifndef @^H_FILE
#define @^H_FILE

#include "global.h"

// ================ Exported functions =============

void*		@_Malloc( const char* const file, const int line, const size_t size );
int			@_IsLittleEndian();

uint64_t	@_ConvertBinary_hton64( const uint64_t x );
uint64_t	@_ConvertBinary_ntoh64( const uint64_t x );

			// Prints in a network order (big-endian)
void		@_PrintHex( const void* const b, const size_t len );

#endif

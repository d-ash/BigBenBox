<?:c:guard ?>

#include "global.h"

<?:prefix @_ bbb_util_ ?>
<?:prefix @^ BBB_UTIL_ ?>

#define @^MALLOC( size )	@_Malloc( __FILE__, __LINE__, ( size ) )

// ================ Exported functions =============

void*		@_Malloc( const char* const file, const int line, const size_t size );
int			@_IsLittleEndian();

uint64_t	@_ConvertBinary_hton64( const uint64_t x );
uint64_t	@_ConvertBinary_ntoh64( const uint64_t x );

			// Prints in a network order (big-endian)
void		@_PrintHex( const void* const b, const size_t len );

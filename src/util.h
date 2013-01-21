#ifndef _BBB_UTIL_H
#define _BBB_UTIL_H

#include "global.h"

// ================ Exported functions =============

int			bbb_util_IsLittleEndian();

uint64_t	bbb_util_ConvertBinary_hton64( const uint64_t x );
uint64_t	bbb_util_ConvertBinary_ntoh64( const uint64_t x );

			// Prints in a network order (big-endian)
void		bbb_util_PrintHex( const void* const b, const size_t len );

#endif

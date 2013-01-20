#ifndef _BBB_UTIL_H
#define _BBB_UTIL_H

#include "global.h"

// ================ Exported functions =============

int			bbb_util_IsLittleEndian();

			// Prints in a network order (big-endian)
void		bbb_util_PrintHex( const void* const b, const size_t len );

#endif

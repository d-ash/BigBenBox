#ifndef _BBB_UTIL_H
#define _BBB_UTIL_H

#include "global.h"

// ================ Exported functions =============

int			BbbUtilIsLittleEndian();

			// Prints in a network order (big-endian)
void		BbbUtilPrintHex( const void* const b, const size_t len );

#endif

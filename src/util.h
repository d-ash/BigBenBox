#ifndef _BBB_UTIL_H
#define _BBB_UTIL_H

#include "global.h"

// ================ Exported functions =============

int			BbbIsLittleEndian();

			// Prints in a network order (big-endian)
void		BbbPrintHex( const void* const b, const size_t len );

#endif

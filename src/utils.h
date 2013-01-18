#ifndef _BBB_UTILS_H
#define _BBB_UTILS_H

#include "global.h"

int			BbbIsLittleEndian();

			// Prints in a network order (big-endian)
void		BbbPrintHex( const void* const b, const size_t len );

#endif

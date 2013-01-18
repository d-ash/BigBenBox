#ifndef _BBB_UTILS_H
#define _BBB_UTILS_H

#include "global.h"

const int			bbbIsLittleEndian();

					// Prints in a network order (big-endian)
void				bbbPrintHex( const void* const b, const size_t len );

#endif

#ifndef _PACK_H
#define _PACK_H

#include "snapshot.h"

// structure of the pack:
// [ PACK: [ HEADER: status_byte, hash, file_size, path_len ], path ]

#define PACK_HEADER_SIZE	(1 + HASH_SUM_LENGTH + sizeof(off_t) + sizeof(size_t))

unsigned char* pack_snapshot(SNAPSHOT* ss);
SNAPSHOT* unpack_snapshot(unsigned char* data);

#endif	// _PACK_H

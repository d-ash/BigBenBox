#ifndef _PACK_H
#define _PACK_H

#include "snapshot.h"

//	[ ENTRY:
//		[ HEADER:
//			status,
//			hash,
//			file_size,
//			path_len
//		],
//		path
//	]
//
//	[ SNAPSHOT:
//		[ ENTRY_1 ]
//		[ ENTRY_2 ]
//		...
//	]

typedef struct s_entry_header {
	uint32_t status;
	unsigned char hash[HASH_SUM_LENGTH];
	uint64_t fsize;
	uint64_t pmem;
} PACK_ENTRY_HEADER;

unsigned char* pack_snapshot(SNAPSHOT* ss);
SNAPSHOT* unpack_snapshot(unsigned char* data);

#endif	// _PACK_H

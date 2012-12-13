#ifndef _PACK_H
#define _PACK_H

#include "snapshot.h"

unsigned char* pack_snapshot(SNAPSHOT* ss);
SNAPSHOT* unpack_snapshot(unsigned char* data);

#endif	// _PACK_H

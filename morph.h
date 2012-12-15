#ifndef _MORPH_H
#define _MORPH_H

/*
#include "snapshot.h"

#define ACTION_CREATE	0x01
#define ACTION_BACKUP	0x02
#define ACTION_COPY		0x03
#define ACTION_MOVE		0x04
#define ACTION_RSYNC	0x05
#define ACTION_DELETE	0x06

typedef struct s_ssaction {
	int type;
	char* key;
	char* to;
	char* src_content;
	char* dst_content;
} SSACTION;

typedef struct s_sspatch {
	SSACTION* actions;
} SSPATCH;

SSPATCH* morph(snapshot_t A, snapshot_t B);
snapshot_t patch(snapshot_t A, SSPATCH* P);

int is_equal(snapshot_t A, snapshot_t B);
*/

#endif	// _MORPH_H

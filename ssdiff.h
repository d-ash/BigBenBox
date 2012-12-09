#ifndef _SSDIFF_H
#define _SSDIFF_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "snapshot.h"

#define	SSACT_TYPE_NEW		0x01
#define	SSACT_TYPE_DELETE	0x02
#define	SSACT_TYPE_COPY		0x03
#define	SSACT_TYPE_MOVE		0x04

typedef struct s_ssfile {
	char* path;
	unsigned char hash[MD5_DIGEST_LENGTH];
	off_t size;
} SSFILE;

typedef struct s_sscontent {
	unsigned char* data;
	off_t len;
} SSCONTENT;

typedef struct s_ssact {
	unsigned char type;
	SSFILE* file;
	SSFILE* file_dst;	// destination parameters for COPY and MOVE actions
	SSCONTENT* content;	// used in CREATE action

	void* next;	// link to the next action
} SSACT;

typedef struct s_sspatch {
	SSACT* actions;
} SSPATCH;

SSPATCH* ssdiff(SNAPSHOT* s1, SNAPSHOT* s2);

#endif	// _SSDIFF_H

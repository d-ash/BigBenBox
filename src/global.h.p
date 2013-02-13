#ifndef _BBB_GLOBAL_H
#define _BBB_GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>			// portable printf() identifiers
#include <sys/stat.h>
#include <netinet/in.h>

// ===========================================

#define BBB_VERSION				"0.1.0"		// J.N.PP (J - major, N - minor, PP - patch)
#define BBB_VERNUM				0x0100		// 0xJNPP
#define BBB_WORD_SIZE			sizeof( size_t )

#undef BBB_PLATFORM_ID

#if BBB_PLATFORM_LINUX
  #define BBB_PLATFORM_ID		0x01
#elif BBB_PLATFORM_OSX
  #define BBB_PLATFORM_ID		0x20
#elif BBB_PLATFORM_WINDOWS
  #define BBB_PLATFORM_ID		0x80
#endif

#ifndef BBB_PLATFORM_ID
  #error Cannot define BBB_PLATFORM_ID
#endif

// =============== Windows ===================

#ifdef BBB_PLATFORM_WINDOWS
  #include <winsock2.h>
  #include "dirent.h"
#else
  #include <dirent.h>
#endif

// ===========================================

#ifdef BBB_DEBUG
  #define BBB_PLOG( ... )		do { fprintf( stderr, __VA_ARGS__ ); } while ( 0 );
#else
  #define BBB_PLOG( ... )		do {} while ( 0 );
#endif

#define BBB_PERR( ... )			do { fprintf( stderr, "ERROR: " ); fprintf( stderr, __VA_ARGS__ ); } while ( 0 );

// ===========================================

typedef unsigned char			bbb_byte_t;

#endif

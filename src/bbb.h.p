#ifndef _BBB_H
#define _BBB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>			// portable printf() types, use ("%" PRIuPTR "\n") for size_t

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

#ifdef BBB_RELEASE
#else
#endif

#ifdef BBB_VERBOSE
  #define BBB_LOG( ... )		do { \
									fprintf( stderr, __VA_ARGS__ ); \
									fprintf( stderr, "\n" ); \
								} while ( 0 );
#else
  #define BBB_LOG( ... )		do {} while ( 0 );
#endif

#define BBB_LOG_ERR( ... )		do { \
									fprintf( stderr, "BigBenBox error: " __VA_ARGS__ ); \
									fprintf( stderr, " ( %s:%d )\n", __FILE__, __LINE__ ); \
								} while ( 0 );

#include "bbb_errors.h"

// ===========================================

typedef int32_t					bbb_result_t;
typedef unsigned char			bbb_byte_t;
typedef uint32_t				bbb_checksum_t;

#ifndef BBB_INSIDE_LIB
  #include "bbb_sshot.h"
  #include "bbb_sshot_file.h"
  #include "bbb_util.h"
  #include "bbb_util_hash.h"
  #include "bbb_bio.h"
#endif

#endif

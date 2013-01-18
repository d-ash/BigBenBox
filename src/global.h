#ifndef _BBB_GLOBAL_H
#define _BBB_GLOBAL_H

#define VERSION				0x0001
#define WORD_SIZE			sizeof( size_t )

// ===========================================

#undef PLATFORM_ID

#if PLATFORM_LINUX
  #define PLATFORM_ID		0x01
#elif PLATFORM_OSX
  #define PLATFORM_ID		0x20
#elif PLATFORM_WINDOWS
  #define PLATFORM_ID		0x80
#endif

#ifndef PLATFORM_ID
  #error Unknown PLATFORM
#endif

// ===========================================

#ifdef DEBUG
  #define PLOG( ... )		do { fprintf( stderr, __VA_ARGS__ ); } while ( 0 );
#else
  #define PLOG( ... )		do {} while ( 0 );
#endif

#define PERR( ... )			do { fprintf( stderr, "ERROR: " ); fprintf( stderr, __VA_ARGS__ ); } while ( 0 );

#endif
/**
 *  Portable binary I/O.
 */

#ifndef _BBB_UTIL_BIO_H
#define _BBB_UTIL_BIO_H

#include "global.h"

// ================= Exported functions ==================

int		BbbUtilBIOWrite_uint8( const uint8_t v, FILE* const f );
int		BbbUtilBIOWrite_uint16( const uint16_t v, FILE* const f );
int		BbbUtilBIOWrite_uint32( const uint32_t v, FILE* const f );
int		BbbUtilBIOWrite_uint64( const uint64_t v, FILE* const f );
int		BbbUtilBIOWrite_buf( const char* const buf, const size_t len, FILE* const f );

int		BbbUtilBIORead_uint8( uint8_t* v, FILE* const f );
int		BbbUtilBIORead_uint16( uint16_t* v, FILE* const f );
int		BbbUtilBIORead_uint32( uint32_t* v, FILE* const f );
int		BbbUtilBIORead_uint64( uint64_t* v, FILE* const f );
int		BbbUtilBIORead_buf( char* const buf, const size_t len, FILE* const f );

#endif

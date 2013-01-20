/**
 *  Portable binary I/O.
 */

#ifndef _BBB_UTIL_BIO_H
#define _BBB_UTIL_BIO_H

#include "global.h"


// ================= Exported functions ==================

size_t	bbb_util_bio_Write_uint16( const uint16_t v, FILE* const f );
size_t	bbb_util_bio_Write_uint32( const uint32_t v, FILE* const f );
size_t	bbb_util_bio_Write_uint64( const uint64_t v, FILE* const f );

size_t	bbb_util_bio_Read_uint16( uint16_t* v, FILE* const f );
size_t	bbb_util_bio_Read_uint32( uint32_t* v, FILE* const f );
size_t	bbb_util_bio_Read_uint64( uint64_t* v, FILE* const f );

#endif

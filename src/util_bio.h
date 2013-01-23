/**
 *  Portable binary I/O.
 */

#ifndef _BBB_UTIL_BIO_H
#define _BBB_UTIL_BIO_H

#include "global.h"

typedef struct {
	uint32_t	len;
	bbb_byte_t*	buf;
} bbb_varbuf_t;

// ================= Exported functions ==================

size_t	bbb_util_bio_Write_uint16( const uint16_t v, FILE* const f );
size_t	bbb_util_bio_Write_uint32( const uint32_t v, FILE* const f );
size_t	bbb_util_bio_Write_uint64( const uint64_t v, FILE* const f );
size_t	bbb_util_bio_Write_varbuf( const bbb_varbuf_t vb, FILE* const f );

size_t	bbb_util_bio_Read_uint16( uint16_t* const v, FILE* const f );
size_t	bbb_util_bio_Read_uint32( uint32_t* const v, FILE* const f );
size_t	bbb_util_bio_Read_uint64( uint64_t* const v, FILE* const f );
size_t	bbb_util_bio_Read_varbuf( bbb_varbuf_t* const vb, FILE* const f );

int		bbb_util_bio_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 );

#endif

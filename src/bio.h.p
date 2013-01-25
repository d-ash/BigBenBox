/**
 *  Portable binary I/O.
 */

#ifndef _BBB_BIO_H
#define _BBB_BIO_H

#include "global.h"
#include "util_hash.h"

typedef struct {
	uint32_t	len;
	bbb_byte_t*	buf;
} bbb_varbuf_t;

/**
 *  Read/Write functions return:
 *		on a success:	a number of BYTES read
 *		on a failure:	0 (values and position in a file are irrelevant)
 *
 *	In case of a failed call to File I/O better check feof() and ferror().
 */

// Buffer I/O

size_t	bbb_bio_WriteToBuf_uint16( const uint16_t v, bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_WriteToBuf_uint32( const uint32_t v, bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_WriteToBuf_uint64( const uint64_t v, bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_WriteToBuf_varbuf( const bbb_varbuf_t vb, bbb_byte_t* const buf, const size_t len );

size_t	bbb_bio_ReadFromBuf_uint16( uint16_t* const v, const bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_ReadFromBuf_uint32( uint32_t* const v, const bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_ReadFromBuf_uint64( uint64_t* const v, const bbb_byte_t* const buf, const size_t len );
size_t	bbb_bio_ReadFromBuf_varbuf( bbb_varbuf_t* const vb, const bbb_byte_t* const buf, const size_t len );

// File I/O

size_t	bbb_bio_WriteToFile_uint16( const uint16_t v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_WriteToFile_uint32( const uint32_t v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_WriteToFile_uint64( const uint64_t v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_WriteToFile_varbuf( const bbb_varbuf_t vb, FILE* const f, bbb_checksum_t* const chk );

size_t	bbb_bio_ReadFromFile_uint16( uint16_t* const v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_ReadFromFile_uint32( uint32_t* const v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_ReadFromFile_uint64( uint64_t* const v, FILE* const f, bbb_checksum_t* const chk );
size_t	bbb_bio_ReadFromFile_varbuf( bbb_varbuf_t* const vb, FILE* const f, bbb_checksum_t* const chk );

int		bbb_bio_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 );

#endif

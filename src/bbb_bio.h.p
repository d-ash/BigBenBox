/**
 *	Portable binary I/O.
 */

<?:prefix @_ bbb_bio_ ?>
<?:prefix @^ BBB_BIO_ ?>

#ifndef @^H
#define @^H

#include "bbb.h"

typedef struct {
	uint32_t	len;
	bbb_byte_t*	buf;
} bbb_varbuf_t;

// Buffer I/O

bbb_result_t	@_WriteToBuf_uint16( const uint16_t v,			bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_WriteToBuf_uint32( const uint32_t v,			bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_WriteToBuf_uint64( const uint64_t v,			bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_WriteToBuf_varbuf( const bbb_varbuf_t vb,		bbb_byte_t* const buf, const size_t len );

bbb_result_t	@_ReadFromBuf_uint16( uint16_t* const v,		const bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_ReadFromBuf_uint32( uint32_t* const v,		const bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_ReadFromBuf_uint64( uint64_t* const v,		const bbb_byte_t* const buf, const size_t len );
bbb_result_t	@_ReadFromBuf_varbuf( bbb_varbuf_t* const vb,	const bbb_byte_t* const buf, const size_t len );

// File I/O

bbb_result_t	@_WriteToFile_uint16( const uint16_t v,			FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_WriteToFile_uint32( const uint32_t v,			FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_WriteToFile_uint64( const uint64_t v,			FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_WriteToFile_varbuf( const bbb_varbuf_t vb,	FILE* const f, bbb_checksum_t* const chk );

bbb_result_t	@_ReadFromFile_uint16( uint16_t* const v,		FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_ReadFromFile_uint32( uint32_t* const v,		FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_ReadFromFile_uint64( uint64_t* const v,		FILE* const f, bbb_checksum_t* const chk );
bbb_result_t	@_ReadFromFile_varbuf( bbb_varbuf_t* const vb,	FILE* const f, bbb_checksum_t* const chk );

int				@_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 );
size_t			@_GetSize_varbuf( const bbb_varbuf_t vb );

#endif

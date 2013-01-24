#include "bio.h"
#include "util.h"

// ============== Writing to a buffer =============

size_t bbb_bio_WriteToBuf_uint16( const uint16_t v, bbb_byte_t* const buf, const size_t len ) {
	uint16_t	t;

	if ( len < sizeof( t ) ) {
		return 0;
	}

	t = htons( v );
	memcpy( buf, &t, sizeof ( t ) );
	return sizeof( t );
}

size_t bbb_bio_WriteToBuf_uint32( const uint32_t v, bbb_byte_t* const buf, const size_t len ) {
	uint32_t	t;

	if ( len < sizeof( t ) ) {
		return 0;
	}

	t = htonl( v );
	memcpy( buf, &t, sizeof( t ) );
	return sizeof( t );
}

size_t bbb_bio_WriteToBuf_uint64( const uint64_t v, bbb_byte_t* const buf, const size_t len ) {
	uint64_t	t;

	if ( len < sizeof( t ) ) {
		return 0;
	}

	t = bbb_util_ConvertBinary_hton64( v );
	memcpy( buf, &t, sizeof( t ) );
	return sizeof( t );
}

size_t bbb_bio_WriteToBuf_varbuf( const bbb_varbuf_t vb, bbb_byte_t* const buf, const size_t len ) {
	if ( len < ( sizeof( vb.len ) + vb.len ) ) {
		return 0;
	}

	if ( bbb_bio_WriteToBuf_uint32( vb.len, buf, sizeof( vb.len ) ) == 0 ) {
		return 0;
	}

	memcpy( buf + sizeof( vb.len ), vb.buf, vb.len );
	return ( sizeof( vb.len ) + vb.len );
}

// ============== Reading from a buffer =============

size_t bbb_bio_ReadFromBuf_uint16( uint16_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	if ( len < sizeof( uint16_t ) ) {
		return 0;
	}

	*v = ntohs( *( uint16_t* ) buf );
	return sizeof( uint16_t );
}

size_t bbb_bio_ReadFromBuf_uint32( uint32_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	if ( len < sizeof( uint32_t ) ) {
		return 0;
	}

	*v = ntohl( *( uint32_t* ) buf );
	return sizeof( uint32_t );
}

size_t bbb_bio_ReadFromBuf_uint64( uint64_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	if ( len < sizeof( uint64_t ) ) {
		return 0;
	}

	*v = bbb_util_ConvertBinary_ntoh64( *( uint64_t* ) buf );
	return sizeof( uint64_t );
}

size_t bbb_bio_ReadFromBuf_varbuf( bbb_varbuf_t* const vb, const bbb_byte_t* const buf, const size_t len ) {
	if ( bbb_bio_ReadFromBuf_uint32( &( vb->len ), buf, len ) == 0 ) {
		return 0;
	}

	if ( len < sizeof( vb->len ) + vb->len ) {
		return 0;
	}

	vb->buf = malloc( vb->len );
	if ( vb->buf == NULL ) {
		return 0;	// ignoring this fatal error
	}

	memcpy( vb->buf, buf + sizeof( vb->len ), vb->len );
	return ( sizeof( vb->len ) + vb->len );
}

// ============== Writing to a file =============

size_t bbb_bio_WriteToFile_uint16( const uint16_t v, FILE* const f, bbb_checksum_t* const chk ) {
	uint16_t	t;

	t = htons( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	return ( fwrite( &t, sizeof( t ), 1, f ) * sizeof( t ) );
}

size_t bbb_bio_WriteToFile_uint32( const uint32_t v, FILE* const f, bbb_checksum_t* const chk ) {
	uint32_t	t;

	t = htonl( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	return ( fwrite( &t, sizeof( t ), 1, f ) * sizeof( t ) );
}

size_t bbb_bio_WriteToFile_uint64( const uint64_t v, FILE* const f, bbb_checksum_t* const chk ) {
	uint64_t	t;

	t = bbb_util_ConvertBinary_hton64( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	return ( fwrite( &t, sizeof( t ), 1, f ) * sizeof( t ) );
}

size_t bbb_bio_WriteToFile_varbuf( const bbb_varbuf_t vb, FILE* const f, bbb_checksum_t* const chk ) {
	if ( bbb_bio_WriteToFile_uint32( vb.len, f, chk ) == 0 ) {
		return 0;
	}

	bbb_util_hash_UpdateChecksum( vb.buf, vb.len, chk );
	return ( fwrite( vb.buf, vb.len, 1, f ) * ( sizeof( vb.len ) + vb.len ) );
}

// ============== Reading from a file =============

size_t bbb_bio_ReadFromFile_uint16( uint16_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	uint16_t	t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		return 0;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = ntohs( t );
	return sizeof( t );
}

size_t bbb_bio_ReadFromFile_uint32( uint32_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	uint32_t	t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		return 0;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = ntohl( t );
	return sizeof( t );
}

size_t bbb_bio_ReadFromFile_uint64( uint64_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	uint64_t	t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		return 0;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = bbb_util_ConvertBinary_ntoh64( t );
	return sizeof( t );
}

size_t bbb_bio_ReadFromFile_varbuf( bbb_varbuf_t* const vb, FILE* const f, bbb_checksum_t* const chk ) {
	if ( bbb_bio_ReadFromFile_uint32( &( vb->len ), f, chk ) == 0 ) {
		return 0;
	}

	vb->buf = malloc( vb->len );
	if ( vb->buf == NULL ) {
		return 0;	// ignoring this fatal error
	}

	if ( fread( vb->buf, vb->len, 1, f ) == 0 ) {
		free( vb->buf );
		return 0;
	}

	bbb_util_hash_UpdateChecksum( vb->buf, vb->len, chk );
	return ( sizeof( vb->len ) + vb->len );
}

// ============== Other functions =============

int bbb_bio_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 ) {
	if ( vb1.len != vb2.len ) {
		return 0;
	}
	return ( memcmp( vb1.buf, vb2.buf, vb1.len ) == 0 );
}

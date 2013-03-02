#include "bbb_bio.h"
#include "bbb_util.h"
#include "bbb_util_hash.h"

// ============== Writing to a buffer =============

bbb_result_t
bbb_bio_WriteToBuf_uint16( const uint16_t v, bbb_byte_t* const buf, const size_t len, size_t* const written ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	t = htons( v );
	memcpy( buf, &t, sizeof ( t ) );

L_end:
	if ( BBB_FAILED( result ) ) {
		*written = 0;
	} else {
		*written = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_WriteToBuf_uint32( const uint32_t v, bbb_byte_t* const buf, const size_t len, size_t* const written ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	t = htonl( v );
	memcpy( buf, &t, sizeof( t ) );

L_end:
	if ( BBB_FAILED( result ) ) {
		*written = 0;
	} else {
		*written = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_WriteToBuf_uint64( const uint64_t v, bbb_byte_t* const buf, const size_t len, size_t* const written ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	t = bbb_util_ConvertBinary_hton64( v );
	memcpy( buf, &t, sizeof( t ) );

L_end:
	if ( BBB_FAILED( result ) ) {
		*written = 0;
	} else {
		*written = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_WriteToBuf_varbuf( const bbb_varbuf_t vb, bbb_byte_t* const buf, const size_t len, size_t* const written ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < ( sizeof( vb.len ) + vb.len ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	if ( BBB_FAILED( result = bbb_bio_WriteToBuf_uint32( vb.len, buf, sizeof( vb.len ), written ) ) ) {
		goto L_end;
	}

	memcpy( buf + sizeof( vb.len ), vb.buf, vb.len );

L_end:
	if ( BBB_FAILED( result ) ) {
		*written = 0;
	} else {
		*written = sizeof( vb.len ) + vb.len;
	}
	return result;
}

// ============== Reading from a buffer =============

bbb_result_t
bbb_bio_ReadFromBuf_uint16( uint16_t* const v, const bbb_byte_t* const buf, const size_t len, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = ntohs( *( uint16_t* ) buf );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( *v );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromBuf_uint32( uint32_t* const v, const bbb_byte_t* const buf, const size_t len, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = ntohl( *( uint32_t* ) buf );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( *v );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromBuf_uint64( uint64_t* const v, const bbb_byte_t* const buf, const size_t len, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = bbb_util_ConvertBinary_ntoh64( *( uint64_t* ) buf );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( *v );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromBuf_varbuf( bbb_varbuf_t* const vb, const bbb_byte_t* const buf, const size_t len, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( BBB_FAILED( result = bbb_bio_ReadFromBuf_uint32( &( vb->len ), buf, len, read ) ) ) {
		goto L_end;
	}

	if ( len < sizeof( vb->len ) + vb->len ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&( vb->buf ), vb->len ) ) ) {
		goto L_end;
	}

	memcpy( vb->buf, buf + sizeof( vb->len ), vb->len );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( vb->len ) + vb->len;
	}
	return result;
}

// ============== Writing to a file =============

bbb_result_t
bbb_bio_WriteToFile_uint16( const uint16_t v, FILE* const f, bbb_checksum_t* const chk, size_t* const written ) {
	uint16_t	t;

	t = htons( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );

	*written = fwrite( &t, sizeof( t ), 1, f ) * sizeof( t );
	if ( *written == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
bbb_bio_WriteToFile_uint32( const uint32_t v, FILE* const f, bbb_checksum_t* const chk, size_t* const written ) {
	uint32_t	t;

	t = htonl( v );
	if ( chk != NULL ) {	// special case for a trailing checksum in files
		bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	}

	*written = fwrite( &t, sizeof( t ), 1, f ) * sizeof( t );
	if ( *written == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
bbb_bio_WriteToFile_uint64( const uint64_t v, FILE* const f, bbb_checksum_t* const chk, size_t* const written ) {
	uint64_t	t;

	t = bbb_util_ConvertBinary_hton64( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );

	*written = fwrite( &t, sizeof( t ), 1, f ) * sizeof( t );
	if ( *written == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		return BBB_ERROR_FILESYSTEMIO;
	}
	return BBB_SUCCESS;
}

bbb_result_t
bbb_bio_WriteToFile_varbuf( const bbb_varbuf_t vb, FILE* const f, bbb_checksum_t* const chk, size_t* const written ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( BBB_FAILED( result = bbb_bio_WriteToFile_uint32( vb.len, f, chk, written ) ) ) {
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( vb.buf, vb.len, chk );

	*written = fwrite( vb.buf, vb.len, 1, f ) * ( sizeof( vb.len ) + vb.len );
	if ( *written == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		goto L_end;
	}

L_end:
	return result;
}

// ============== Reading from a file =============

bbb_result_t
bbb_bio_ReadFromFile_uint16( uint16_t* const v, FILE* const f, bbb_checksum_t* const chk, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = ntohs( t );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromFile_uint32( uint32_t* const v, FILE* const f, bbb_checksum_t* const chk, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		goto L_end;
	}

	if ( chk != NULL ) {	// special case for a trailing checksum in files
		bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	}
	*v = ntohl( t );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromFile_uint64( uint64_t* const v, FILE* const f, bbb_checksum_t* const chk, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;

	if ( fread( &t, sizeof( t ), 1, f ) == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = bbb_util_ConvertBinary_ntoh64( t );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( t );
	}
	return result;
}

bbb_result_t
bbb_bio_ReadFromFile_varbuf( bbb_varbuf_t* const vb, FILE* const f, bbb_checksum_t* const chk, size_t* const read ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( BBB_FAILED( result = bbb_bio_ReadFromFile_uint32( &( vb->len ), f, chk, read ) ) ) {
		goto L_end;
	}

	if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&( vb->buf ), vb->len ) ) ) {
		goto L_end;
	}
	if ( fread( vb->buf, vb->len, 1, f ) == 0 ) {
		free( vb->buf );
		BBB_ERR_CODE( BBB_ERROR_FILESYSTEMIO, "%s", strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( vb->buf, vb->len, chk );

L_end:
	if ( BBB_FAILED( result ) ) {
		*read = 0;
	} else {
		*read = sizeof( vb->len ) + vb->len;
	}
	return result;
}

// ============== Other functions =============

int
bbb_bio_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 ) {
	if ( vb1.len != vb2.len ) {
		return 0;
	}
	return ( memcmp( vb1.buf, vb2.buf, vb1.len ) == 0 );
}

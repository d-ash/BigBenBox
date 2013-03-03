<?:include c_lang.p ?>

#include "bbb_bio.h"
#include "bbb_util.h"
#include "bbb_util_hash.h"

<?:prefix @_ bbb_bio_ ?>
<?:prefix @^ BBB_BIO_ ?>

// ============== Writing to a buffer =============

bbb_result_t
@_WriteToBuf_uint16( const uint16_t v, bbb_byte_t* const buf, const size_t len ) {
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
	return result;
}

bbb_result_t
@_WriteToBuf_uint32( const uint32_t v, bbb_byte_t* const buf, const size_t len ) {
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
	return result;
}

bbb_result_t
@_WriteToBuf_uint64( const uint64_t v, bbb_byte_t* const buf, const size_t len ) {
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
	return result;
}

bbb_result_t
@_WriteToBuf_varbuf( const bbb_varbuf_t vb, bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < ( sizeof( vb.len ) + vb.len ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	if ( BBB_FAILED( result = @_WriteToBuf_uint32( vb.len, buf, sizeof( vb.len ) ) ) ) {
		goto L_end;
	}

	memcpy( buf + sizeof( vb.len ), vb.buf, vb.len );

L_end:
	return result;
}

// ============== Reading from a buffer =============

bbb_result_t
@_ReadFromBuf_uint16( uint16_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = ntohs( *( uint16_t* ) buf );

L_end:
	return result;
}

bbb_result_t
@_ReadFromBuf_uint32( uint32_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = ntohl( *( uint32_t* ) buf );

L_end:
	return result;
}

bbb_result_t
@_ReadFromBuf_uint64( uint64_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		goto L_end;
	}

	*v = bbb_util_ConvertBinary_ntoh64( *( uint64_t* ) buf );

L_end:
	return result;
}

bbb_result_t
@_ReadFromBuf_varbuf( bbb_varbuf_t* const vb, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( BBB_FAILED( result = @_ReadFromBuf_uint32( &( vb->len ), buf, len ) ) ) {
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
	return result;
}

// ============== Writing to a file =============

bbb_result_t
@_WriteToFile_uint16( const uint16_t v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;

	t = htons( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );

	if ( BBB_FAILED( result = bbb_util_Fwrite( &t, sizeof( t ), 1, f ) ) ) {
		goto L_end;
	}

L_end:
	return result;
}

bbb_result_t
@_WriteToFile_uint32( const uint32_t v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;

	t = htonl( v );
	if ( chk != NULL ) {	// special case for a trailing checksum in files
		bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	}

	if ( BBB_FAILED( result = bbb_util_Fwrite( &t, sizeof( t ), 1, f ) ) ) {
		goto L_end;
	}

L_end:
	return result;
}

bbb_result_t
@_WriteToFile_uint64( const uint64_t v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;

	t = bbb_util_ConvertBinary_hton64( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );

	if ( BBB_FAILED( result = bbb_util_Fwrite( &t, sizeof( t ), 1, f ) ) ) {
		goto L_end;
	}

L_end:
	return result;
}

bbb_result_t
@_WriteToFile_varbuf( const bbb_varbuf_t vb, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( BBB_FAILED( result = @_WriteToFile_uint32( vb.len, f, chk ) ) ) {
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( vb.buf, vb.len, chk );

	if ( BBB_FAILED( result = bbb_util_Fwrite( vb.buf, vb.len, 1, f ) ) ) {
		goto L_end;
	}

L_end:
	return result;
}

// ============== Reading from a file =============

bbb_result_t
@_ReadFromFile_uint16( uint16_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;
	size_t			read;

	if ( BBB_FAILED( result = bbb_util_Fread( &t, sizeof( t ), 1, f, &read ) ) ) {
		goto L_end;
	}
	if ( read == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = ntohs( t );

L_end:
	return result;
}

bbb_result_t
@_ReadFromFile_uint32( uint32_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;
	size_t			read;

	if ( BBB_FAILED( result = bbb_util_Fread( &t, sizeof( t ), 1, f, &read ) ) ) {
		goto L_end;
	}
	if ( read == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		goto L_end;
	}

	if ( chk != NULL ) {	// special case for a trailing checksum in files
		bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	}
	*v = ntohl( t );

L_end:
	return result;
}

bbb_result_t
@_ReadFromFile_uint64( uint64_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;
	size_t			read;

	if ( BBB_FAILED( result = bbb_util_Fread( &t, sizeof( t ), 1, f, &read ) ) ) {
		goto L_end;
	}
	if ( read == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		goto L_end;
	}

	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = bbb_util_ConvertBinary_ntoh64( t );

L_end:
	return result;
}

bbb_result_t
@_ReadFromFile_varbuf( bbb_varbuf_t* const vb, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	size_t			read;

	if ( BBB_FAILED( result = @_ReadFromFile_uint32( &( vb->len ), f, chk ) ) ) {
		<? c_GotoCleanup(); ?>
	}

	if ( BBB_FAILED( result = bbb_util_Malloc( ( void** )&( vb->buf ), vb->len ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	<? c_OnCleanup( "?>
		if ( BBB_FAILED( result ) ) {
			free( vb->buf );
		}
	<?" ); ?>

	if ( BBB_FAILED( result = bbb_util_Fread( vb->buf, vb->len, 1, f, &read ) ) ) {
		<? c_GotoCleanup(); ?>
	}
	if ( read == 0 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}

	bbb_util_hash_UpdateChecksum( vb->buf, vb->len, chk );

	<? c_Cleanup(); ?>
	return result;
}

// ============== Other functions =============

int
@_IsEqual_varbuf( const bbb_varbuf_t vb1, const bbb_varbuf_t vb2 ) {
	if ( vb1.len != vb2.len ) {
		return 0;
	}
	return ( memcmp( vb1.buf, vb2.buf, vb1.len ) == 0 );
}

size_t
@_GetSize_varbuf( const bbb_varbuf_t vb ) {
	return ( sizeof( vb.len ) + vb.len );
}

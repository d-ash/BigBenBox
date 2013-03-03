<?:include bbb.p ?>
<?:prefix @_ bbb_bio_ ?>
<?:prefix @^ BBB_BIO_ ?>

#include "bbb_bio.h"
#include "bbb_util.h"
#include "bbb_util_hash.h"

// ============== Writing to a buffer =============

bbb_result_t
@_WriteToBuf_uint16( const uint16_t v, bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	t = htons( v );
	memcpy( buf, &t, sizeof ( t ) );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_WriteToBuf_uint32( const uint32_t v, bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	t = htonl( v );
	memcpy( buf, &t, sizeof( t ) );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_WriteToBuf_uint64( const uint64_t v, bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;

	if ( len < sizeof( t ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	t = bbb_util_ConvertBinary_hton64( v );
	memcpy( buf, &t, sizeof( t ) );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_WriteToBuf_varbuf( const bbb_varbuf_t vb, bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < ( sizeof( vb.len ) + vb.len ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	<? bbb_Call( "?> @_WriteToBuf_uint32( vb.len, buf, sizeof( vb.len ) ) <?" ); ?>
	memcpy( buf + sizeof( vb.len ), vb.buf, vb.len );

	<? c_Cleanup(); ?>
	return result;
}

// ============== Reading from a buffer =============

bbb_result_t
@_ReadFromBuf_uint16( uint16_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	*v = ntohs( *( uint16_t* ) buf );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromBuf_uint32( uint32_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	*v = ntohl( *( uint32_t* ) buf );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromBuf_uint64( uint64_t* const v, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	if ( len < sizeof( *v ) ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	*v = bbb_util_ConvertBinary_ntoh64( *( uint64_t* ) buf );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromBuf_varbuf( bbb_varbuf_t* const vb, const bbb_byte_t* const buf, const size_t len ) {
	bbb_result_t	result = BBB_SUCCESS;

	<? bbb_Call( "?> @_ReadFromBuf_uint32( &( vb->len ), buf, len ) <?" ); ?>
	if ( len < sizeof( vb->len ) + vb->len ) {
		BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER );
		result = BBB_ERROR_SMALLBUFFER;
		<? c_GotoCleanup(); ?>
	}

	<? bbb_Call( "?> bbb_util_Malloc( ( void** )&( vb->buf ), vb->len ) <?" ); ?>
	memcpy( vb->buf, buf + sizeof( vb->len ), vb->len );

	<? c_Cleanup(); ?>
	return result;
}

// ============== Writing to a file =============

bbb_result_t
@_WriteToFile_uint16( const uint16_t v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;

	t = htons( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	<? bbb_Call( "?> bbb_util_Fwrite( &t, sizeof( t ), 1, f ) <?" ); ?>

	<? c_Cleanup(); ?>
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
	<? bbb_Call( "?> bbb_util_Fwrite( &t, sizeof( t ), 1, f ) <?" ); ?>

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_WriteToFile_uint64( const uint64_t v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;

	t = bbb_util_ConvertBinary_hton64( v );
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	<? bbb_Call( "?> bbb_util_Fwrite( &t, sizeof( t ), 1, f ) <?" ); ?>

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_WriteToFile_varbuf( const bbb_varbuf_t vb, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;

	<? bbb_Call( "?> @_WriteToFile_uint32( vb.len, f, chk ) <?" ); ?>
	bbb_util_hash_UpdateChecksum( vb.buf, vb.len, chk );
	<? bbb_Call( "?> bbb_util_Fwrite( vb.buf, vb.len, 1, f ) <?" ); ?>

	<? c_Cleanup(); ?>
	return result;
}

// ============== Reading from a file =============

bbb_result_t
@_ReadFromFile_uint16( uint16_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint16_t		t;
	size_t			wasRead;

	<? bbb_Call( "?> bbb_util_Fread( &t, sizeof( t ), 1, f, &wasRead ) <?" ); ?>
	if ( wasRead != 1 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = ntohs( t );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromFile_uint32( uint32_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint32_t		t;
	size_t			wasRead;

	<? bbb_Call( "?> bbb_util_Fread( &t, sizeof( t ), 1, f, &wasRead ) <?" ); ?>
	if ( wasRead != 1 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}
	if ( chk != NULL ) {					// a special case for checksums
		bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	}
	*v = ntohl( t );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromFile_uint64( uint64_t* const v, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	uint64_t		t;
	size_t			wasRead;

	<? bbb_Call( "?> bbb_util_Fread( &t, sizeof( t ), 1, f, &wasRead ) <?" ); ?>
	if ( wasRead != 1 ) {
		BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
		result = BBB_ERROR_CORRUPTEDDATA;
		<? c_GotoCleanup(); ?>
	}
	bbb_util_hash_UpdateChecksum( &t, sizeof( t ), chk );
	*v = bbb_util_ConvertBinary_ntoh64( t );

	<? c_Cleanup(); ?>
	return result;
}

bbb_result_t
@_ReadFromFile_varbuf( bbb_varbuf_t* const vb, FILE* const f, bbb_checksum_t* const chk ) {
	bbb_result_t	result = BBB_SUCCESS;
	size_t			wasRead;

	<? bbb_Call( "?> @_ReadFromFile_uint32( &( vb->len ), f, chk ) <?" ); ?>

	<? bbb_Call( "?> bbb_util_Malloc( ( void** )&( vb->buf ), vb->len ) <?" ); ?>
	<? c_OnCleanup( "?>
		if ( BBB_FAILED( result ) ) {
			free( vb->buf );
		}
	<?" ); ?>

	<? bbb_Call( "?> bbb_util_Fread( vb->buf, vb->len, 1, f, &wasRead ) <?" ); ?>
	if ( wasRead != 1 ) {
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

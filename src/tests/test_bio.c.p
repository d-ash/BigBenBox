#include "test_bio.h"

// WARNING: Including .c file here because it will be easier to build an executable.
#include "test_bio.bio.c"

static int	_testsRun = 0;

static char* _TestBioBufTypes() {
	static const bbb_byte_t	data[ 20 + 14 ] = {
		0x00, 0x00, 0x00, 0x10,		// length of the following seria
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x01, 0x02,					// c16
		0x03, 0x04, 0x05, 0x06,		// c32
		0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14	// c64
	};
	bbb_varbuf_t			vb;
	static const uint16_t	c16 = 0x0102;
	static const uint32_t	c32 = 0x03040506;
	static const uint64_t	c64 = 0x0708091011121314;
	bbb_byte_t		xdata[ 34 ];
	bbb_varbuf_t	xvb;
	uint16_t		x16;
	uint32_t		x32;
	uint64_t		x64;

	vb.len = 16;
	if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( vb.buf ), vb.len ) ) ) {
		exit( 1 );
	}
	memcpy( vb.buf, data + 4, vb.len );

	// Testing of writing.
	MU_ASSERT( "Cannot write to a buffer varbuf_t", bbb_bio_WriteToBuf_varbuf( vb, xdata, 20 ) );
	MU_ASSERT( "Cannot write to a buffer uint16_t", bbb_bio_WriteToBuf_uint16( c16, xdata + 20, 2 ) );
	MU_ASSERT( "Cannot write to a buffer uint32_t", bbb_bio_WriteToBuf_uint32( c32, xdata + 22, 4 ) );
	MU_ASSERT( "Cannot write to a buffer uint64_t", bbb_bio_WriteToBuf_uint64( c64, xdata + 26, 8 ) );
	MU_ASSERT( "bbb_bio_WriteToBuf_...() failed", memcmp( xdata, data, sizeof( data ) ) == 0 );

	// Testing of reading.
	MU_ASSERT( "Cannot read from a buffer varbuf_t", bbb_bio_ReadFromBuf_varbuf( &xvb, xdata, 20 ) );
	MU_ASSERT( "bbb_bio_ReadFromBuf_varbuf() failed", bbb_bio_IsEqual_varbuf( xvb, vb ) );

	MU_ASSERT( "Cannot read from a buffer uint16_t", bbb_bio_ReadFromBuf_uint16( &x16, xdata + 20, 2 ) );
	MU_ASSERT( "bbb_bio_ReadFromBuf_uint16() failed", x16 == c16 );
	MU_ASSERT( "Cannot read from a buffer uint32_t", bbb_bio_ReadFromBuf_uint32( &x32, xdata + 22, 4 ) );
	MU_ASSERT( "bbb_bio_ReadFromBuf_uint32() failed", x32 == c32 );
	MU_ASSERT( "Cannot read from a buffer uint64_t", bbb_bio_ReadFromBuf_uint64( &x64, data + 26, 8 ) );
	MU_ASSERT( "bbb_bio_ReadFromBuf_uint64() failed", x64 == c64 );

	free( xvb.buf );

	MU_ASSERT( "Error is not checked correctly (record)", bbb_bio_ReadFromBuf_varbuf( &xvb, xdata, 19 ) == 0 );

	free( vb.buf );
	vb.len = 0;
	return 0;
}

static char* _TestBioBufRecords() {
	static const char const str[ 5 ] = "ABCD";
	static const bbb_byte_t	data[ 24 ] = {
		0x01,
		0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
		0x00, 0x00, 0x00, 0x05,		// length of the following seria
		'A', 'B', 'C', 'D', '\0'
	};
	bbb_byte_t				xdata[ 24 * 3 ];
	test_bio_fileHeader_t	hdr[ 3 ];
	test_bio_fileHeader_t	xhdr[ 3 ];

	hdr[ 0 ].theFirst = 0x01;
	hdr[ 0 ].theSecond = 0x0203;
	hdr[ 0 ].theThird = 0x04050607;
	hdr[ 0 ].theFourth = 0x0809101112131415;
	hdr[ 0 ].var_buf_777.len = strlen( str ) + 1;
	hdr[ 0 ].var_buf_777.buf = ( bbb_byte_t* ) strdup( str );

	test_bio_Copy_fileHeader( &( hdr[ 1 ] ), &( hdr[ 0 ] ) );
	test_bio_Copy_fileHeader( &( hdr[ 2 ] ), &( hdr[ 0 ] ) );

	// Testing of writing.
	MU_ASSERT( "Cannot write a record to a buffer", test_bio_WriteToBuf_fileHeader( &( hdr[ 0 ] ), xdata, 24 ) );
	MU_ASSERT( "Writing to a buffer failed", memcmp( xdata, data, 24 ) == 0 );
	MU_ASSERT( "Cannot write array of records to a buffer", test_bio_WriteToBufArray_fileHeader( hdr, 3, xdata, 24 * 3 ) );

	// Testing of reading.
	MU_ASSERT( "Cannot read a record from a buffer", test_bio_ReadFromBuf_fileHeader( &( xhdr[ 0 ] ), xdata, 24 ) );
	MU_ASSERT( "Reading from a buffer failed (0)", test_bio_IsEqual_fileHeader( &( xhdr[ 0 ] ), &( hdr[ 0 ] ) ) );
	test_bio_Destroy_fileHeader( &( xhdr[ 0 ] ) );
	MU_ASSERT( "Cannot read array of records from a buffer", test_bio_ReadFromBufArray_fileHeader( xhdr, 3, xdata, 24 * 3 ) );
	MU_ASSERT( "Reading from a buffer failed (0)", test_bio_IsEqual_fileHeader( &( xhdr[ 0 ] ), &( hdr[ 0 ] ) ) );
	MU_ASSERT( "Reading from a buffer failed (1)", test_bio_IsEqual_fileHeader( &( xhdr[ 1 ] ), &( hdr[ 1 ] ) ) );
	MU_ASSERT( "Reading from a buffer failed (2)", test_bio_IsEqual_fileHeader( &( xhdr[ 2 ] ), &( hdr[ 2 ] ) ) );

	test_bio_DestroyEach_fileHeader( xhdr, 3 );

	MU_ASSERT( "Error is not checked correctly (array)", test_bio_ReadFromBufArray_fileHeader( xhdr, 3, xdata, 24 * 3 - 1 ) == 0 );
	MU_ASSERT( "Varbufs are not freed correctly (array)",
		xhdr[ 0 ].var_buf_777.len == 0 && xhdr[ 1 ].var_buf_777.len == 0
		&& xhdr[ 0 ].var_buf_777.buf == NULL && xhdr[ 1 ].var_buf_777.buf == NULL );

	test_bio_DestroyEach_fileHeader( hdr, 3 );
	return 0;
}

static char* _TestBioFileTypes() {
	static const char		path[ 19 ] = "_test_bio.bin";
	static const bbb_byte_t	data[ 34 ] = {
		0x00, 0x00, 0x00, 0x10,		// length of the following seria
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x01, 0x02,					// c16
		0x03, 0x04, 0x05, 0x06,		// c32
		0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14	// c64
	};
	bbb_varbuf_t			vb;
	static const uint16_t	c16 = 0x0102;
	static const uint32_t	c32 = 0x03040506;
	static const uint64_t	c64 = 0x0708091011121314;
	bbb_byte_t		xdata[ 34 ];
	bbb_varbuf_t	xvb;
	uint16_t		x16;
	uint32_t		x32;
	uint64_t		x64;
	FILE*			f;
	bbb_checksum_t	chk = 0;
	bbb_checksum_t	xchk = 0;

	vb.len = 16;
	if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( vb.buf ), vb.len ) ) ) {
		exit( 1 );
	}
	memcpy( vb.buf, data + 4, vb.len );

	// Testing of writing.
	f = fopen( path, "wb" );
	MU_ASSERT( "Cannot create a data file", f != NULL );
	MU_ASSERT( "Cannot write varbuf_t", bbb_bio_WriteToFile_varbuf( vb, f, &chk ) );
	MU_ASSERT( "Cannot write uint16_t", bbb_bio_WriteToFile_uint16( c16, f, &chk ) );
	MU_ASSERT( "Cannot write uint32_t", bbb_bio_WriteToFile_uint32( c32, f, &chk ) );
	MU_ASSERT( "Cannot write uint64_t", bbb_bio_WriteToFile_uint64( c64, f, &chk ) );
	MU_ASSERT( "Cannot close a data file", fclose( f ) == 0 );

	f = fopen( path, "rb" );
	MU_ASSERT( "Cannot read a data file", fread( xdata, sizeof( xdata ), 1, f ) == 1 );
	MU_ASSERT( "bbb_bio_WriteToFile_...() failed", memcmp( xdata, data, sizeof( data ) ) == 0 );
	bbb_util_hash_UpdateChecksum( xdata, sizeof( xdata ), &xchk);
	MU_ASSERT( "Checksums failed (0)", xchk == chk );

	// Testing of reading.
	xchk = 0;
	fseek( f, 0, SEEK_SET );
	MU_ASSERT( "Cannot read varbuf_t", bbb_bio_ReadFromFile_varbuf( &xvb, f, &xchk ) );
	MU_ASSERT( "bbb_bio_ReadFromFile_varbuf() failed", bbb_bio_IsEqual_varbuf( xvb, vb ) );

	MU_ASSERT( "Cannot read uint16_t", bbb_bio_ReadFromFile_uint16( &x16, f, &xchk ) );
	MU_ASSERT( "bbb_bio_ReadFromFile_uint16() failed", x16 == c16 );
	MU_ASSERT( "Cannot read uint32_t", bbb_bio_ReadFromFile_uint32( &x32, f, &xchk ) );
	MU_ASSERT( "bbb_bio_ReadFromFile_uint32() failed", x32 == c32 );
	MU_ASSERT( "Cannot read uint64_t", bbb_bio_ReadFromFile_uint64( &x64, f, &xchk ) );
	MU_ASSERT( "bbb_bio_ReadFromFile_uint64() failed", x64 == c64 );
	fclose( f );

	MU_ASSERT( "Checksums failed (1)", xchk == chk );

	free( xvb.buf );
	free( vb.buf );
	vb.len = 0;
	unlink( path );
	return 0;
}

static char* _TestBioFileRecords() {
	static const char			path[ 27 ] = "_test_bio.records.bin";
	static const size_t			pathlen = 26;
	test_bio_fileHeader_t	hdr;
	test_bio_fileHeader_t	xhdr;
	test_bio_ext333_t		ext[ 3 ];
	test_bio_ext333_t		xext[ 3 ];
	FILE*			f;
	bbb_checksum_t	chk = 0;
	bbb_checksum_t	xchk = 0;

	hdr.theFirst = 0x01;
	hdr.theSecond = 0x0203;
	hdr.theThird = 0x04050607;
	hdr.theFourth = 0x0809101112131415;
	hdr.var_buf_777.len = pathlen + 1;
	hdr.var_buf_777.buf = ( bbb_byte_t* ) strdup( path );

	ext[ 0 ].v1.len = 4;
	if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( ext[ 0 ].v1.buf ), 4 ) ) ) {
		exit( 1 );
	}
	memcpy( ext[ 0 ].v1.buf, &( hdr.theThird ), 4 );
	ext[ 0 ].v2.len = pathlen + 1;
	ext[ 0 ].v2.buf = ( bbb_byte_t* ) strdup( path );
	MU_ASSERT( "Cannot copy ext333 (1)", test_bio_Copy_ext333( &( ext[ 1 ] ), &( ext[ 0 ] ) ) );
	MU_ASSERT( "Cannot copy ext333 (2)", test_bio_Copy_ext333( &( ext[ 2 ] ), &( ext[ 0 ] ) ) );

	MU_ASSERT( "test_bio_GetSize_...() failed", test_bio_GetSize_ext333( &( ext[ 2 ] ) ) == 13 + pathlen );
	MU_ASSERT( "test_bio_GetSizeArray_...() failed", test_bio_GetSizeArray_ext333( ext, 3 ) == 3 * ( 13 + pathlen ) );

	// Testing of writing.
	f = fopen( path, "wb" );
	MU_ASSERT( "Cannot create a data file", f != NULL );
	MU_ASSERT( "Cannot write a record", test_bio_WriteToFile_fileHeader( &hdr, f, &chk ) );
	MU_ASSERT( "Cannot write array of records", test_bio_WriteToFileArray_ext333( ext, 3, f, &chk ) );
	MU_ASSERT( "Cannot close a data file", fclose( f ) == 0 );

	// Testing of reading.
	f = fopen( path, "rb" );
	MU_ASSERT( "Cannot read a record", test_bio_ReadFromFile_fileHeader( &xhdr, f, &xchk ) );
	MU_ASSERT( "fileHeader I/O failed", test_bio_IsEqual_fileHeader( &xhdr, &hdr ) );
	MU_ASSERT( "Cannot read array of records", test_bio_ReadFromFileArray_ext333( xext, 3, f, &xchk ) );
	MU_ASSERT( "ext333 I/O failed (0)", test_bio_IsEqual_ext333( &( xext[ 0 ] ), &( xext[ 0 ] ) ) );
	MU_ASSERT( "ext333 I/O failed (1)", test_bio_IsEqual_ext333( &( xext[ 1 ] ), &( xext[ 1 ] ) ) );
	MU_ASSERT( "ext333 I/O failed (2)", test_bio_IsEqual_ext333( &( xext[ 2 ] ), &( xext[ 2 ] ) ) );
	fclose( f );

	MU_ASSERT( "Checksums failed (2)", xchk == chk );

	test_bio_DestroyEach_ext333( xext, 3 );
	test_bio_DestroyEach_ext333( ext, 3 );
	test_bio_Destroy_fileHeader( &xhdr );
	test_bio_Destroy_fileHeader( &hdr );

	unlink( path );
	return 0;
}

// ================================================

static char* _AllTests() {
	MU_RUN_TEST( _TestBioBufTypes );
	MU_RUN_TEST( _TestBioBufRecords );
	MU_RUN_TEST( _TestBioFileTypes );
	MU_RUN_TEST( _TestBioFileRecords );
	return 0;
}

MAIN

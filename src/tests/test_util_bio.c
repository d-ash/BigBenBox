#include "test_util_bio.h"
#include "util_bio.h"

// WARNING: Including .c file because it will be easier to build an executable.
#include "test_util_bio.bio.c"

static int	_testsRun = 0;

static char* _TestBioTypes() {
	static const char		path[ 19 ] = "_test_util_bio.bin";
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
	bbb_byte_t		x[ 34 ];
	bbb_varbuf_t	xvb;
	uint16_t		x16;
	uint32_t		x32;
	uint64_t		x64;
	FILE*			f;

	vb.len = 16;
	vb.buf = malloc( vb.len );
	memcpy( vb.buf, data + 4, vb.len );

	// Testing of writing.
	f = fopen( path, "wb" );
	MU_ASSERT( "Cannot create a data file", f != NULL );
	MU_ASSERT( "Cannot write varbuf", bbb_util_bio_Write_varbuf( vb, f ) );
	MU_ASSERT( "Cannot write uint16_t", bbb_util_bio_Write_uint16( c16, f ) );
	MU_ASSERT( "Cannot write uint32_t", bbb_util_bio_Write_uint32( c32, f ) );
	MU_ASSERT( "Cannot write uint64_t", bbb_util_bio_Write_uint64( c64, f ) );
	MU_ASSERT( "Cannot close a data file", fclose( f ) == 0 );

	f = fopen( path, "rb" );
	MU_ASSERT( "Cannot read a data file", fread( x, sizeof( x ), 1, f ) == 1 );
	MU_ASSERT( "bbb_util_bio_Write_...() failed", memcmp( x, data, sizeof( data ) ) == 0 );

	// Testing of reading.
	fseek( f, 0, SEEK_SET );
	MU_ASSERT( "Cannot read varbuf", bbb_util_bio_Read_varbuf( &xvb, f ) );
	MU_ASSERT( "bbb_util_bio_Read_varbuf() failed", bbb_util_bio_IsEqual_varbuf( xvb, vb ) );

	MU_ASSERT( "Cannot read uint16_t", bbb_util_bio_Read_uint16( &x16, f ) );
	MU_ASSERT( "bbb_util_bio_Read_uint16() failed", x16 == c16 );
	MU_ASSERT( "Cannot read uint32_t", bbb_util_bio_Read_uint32( &x32, f ) );
	MU_ASSERT( "bbb_util_bio_Read_uint32() failed", x32 == c32 );
	MU_ASSERT( "Cannot read uint64_t", bbb_util_bio_Read_uint64( &x64, f ) );
	MU_ASSERT( "bbb_util_bio_Read_uint64() failed", x64 == c64 );
	fclose( f );

	free( xvb.buf );
	free( vb.buf );
	vb.len = 0;
	unlink( path );
	return 0;
}

static char* _TestBioRecords() {
	static const char			path[ 27 ] = "_test_util_bio.records.bin";
	static const size_t			pathlen = 26;
	test_util_bio_fileHeader_t	hdr;
	test_util_bio_fileHeader_t	xhdr;
	test_util_bio_ext333_t		ext[ 3 ];
	test_util_bio_ext333_t		xext[ 3 ];
	FILE*	f;

	hdr.theFirst = 0x01;
	hdr.theSecond = 0x0203;
	hdr.theThird = 0x04050607;
	hdr.theFourth = 0x0809101112131415;
	hdr.var_buf_777.len = pathlen + 1;
	hdr.var_buf_777.buf = ( bbb_byte_t* ) strdup( path );

	ext[ 0 ].v1.len = 4;
	ext[ 0 ].v1.buf = ( bbb_byte_t* ) malloc( 4 );
	memcpy( ext[ 0 ].v1.buf, &( hdr.theThird ), 4 );
	ext[ 0 ].v2.len = pathlen + 1;
	ext[ 0 ].v2.buf = ( bbb_byte_t* ) strdup( path );
	MU_ASSERT( "Cannot copy ext333 (1)", test_util_bio_Copy_ext333( &( ext[ 1 ] ), &( ext[ 0 ] ) ) );
	MU_ASSERT( "Cannot copy ext333 (2)", test_util_bio_Copy_ext333( &( ext[ 2 ] ), &( ext[ 0 ] ) ) );

	MU_ASSERT( "test_util_bio_GetSize_...() failed", test_util_bio_GetSize_ext333( &( ext[ 2 ] ) ) == 13 + pathlen );

	// Testing of writing.
	f = fopen( path, "wb" );
	MU_ASSERT( "Cannot create a data file", f != NULL );
	MU_ASSERT( "Cannot write a record", test_util_bio_Write_fileHeader( &hdr, f ) );
	MU_ASSERT( "Cannot write array of records", test_util_bio_WriteArray_ext333( ext, 3, f ) );
	MU_ASSERT( "Cannot close a data file", fclose( f ) == 0 );

	// Testing of reading.
	f = fopen( path, "rb" );
	MU_ASSERT( "Cannot read a record", test_util_bio_Read_fileHeader( &xhdr, f ) );
	MU_ASSERT( "fileHeader I/O failed", test_util_bio_IsEqual_fileHeader( &xhdr, &hdr ) );
	MU_ASSERT( "Cannot read array of records", test_util_bio_ReadArray_ext333( xext, 3, f ) );
	MU_ASSERT( "ext333 I/O failed (0)", test_util_bio_IsEqual_ext333( &( xext[ 0 ] ), &( xext[ 0 ] ) ) );
	MU_ASSERT( "ext333 I/O failed (1)", test_util_bio_IsEqual_ext333( &( xext[ 1 ] ), &( xext[ 1 ] ) ) );
	MU_ASSERT( "ext333 I/O failed (2)", test_util_bio_IsEqual_ext333( &( xext[ 2 ] ), &( xext[ 2 ] ) ) );
	fclose( f );

	test_util_bio_DestroyEach_ext333( xext, 3 );
	test_util_bio_DestroyEach_ext333( ext, 3 );
	test_util_bio_Destroy_fileHeader( &xhdr );
	test_util_bio_Destroy_fileHeader( &hdr );

	unlink( path );
	return 0;
}

// ================================================

static char* _AllTests() {
	MU_RUN_TEST( _TestBioTypes );
	MU_RUN_TEST( _TestBioRecords );
	return 0;
}

MAIN

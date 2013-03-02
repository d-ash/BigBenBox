#ifndef _BBB_ERRORS_H
#define _BBB_ERRORS_H

#define BBB_ERROR_NOMEMORY				1
#define BBB_ERROR_FILESYSTEMIO			2
#define BBB_ERROR_DATAISBAD				3


// ================================================================================

#define BBB_SUCCESS						0
#define BBB_FAILED( call )				( ( call ) != BBB_SUCCESS )

#define BBB_ERR( ... )					do { \
											fprintf( stderr, "BigBenBox error: " __VA_ARGS__ ); \
											fprintf( stderr, " ( %s:%u )\n", __FILE__, __LINE__ ); \
										} while ( 0 );

#define BBB_ERR_STR( code, str )		do { \
											fprintf( stderr, "BigBenBox error # %u: %s", ( code ), ( str ) ); \
											fprintf( stderr, " ( %s:%d )\n", __FILE__, __LINE__ ); \
										} while ( 0 );

#endif

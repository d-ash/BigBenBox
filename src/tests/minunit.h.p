/*
 * Minimal unit testing "framework".
 * Got it from: http://www.jera.com/techinfo/jtns/jtn002.html
 */

#ifndef _TESTS_MINUNIT_H
#define _TESTS_MINUNIT_H

#define MU_ASSERT(message, test)	do { if ( !( test ) ) return message; } while ( 0 )
#define MU_RUN_TEST(test)			do { char* message = test(); _testsRun++; \
											if ( message ) return message; } while ( 0 )
#define DATA_DIR	"../../../test_data"

#define MAIN \
	int main() { \
		char*	result; \
	\
		result = _AllTests(); \
	\
		if ( result == 0 ) { \
			printf( "--------------------------------------------- OK\n" ); \
		} else { \
			printf( "--------------------------------------------- >>>>>>>> FAILED: %s\n", result ); \
		} \
	\
		return ( int ) ( result != 0 ); \
	}

#endif

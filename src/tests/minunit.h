// Minimal unit testing "framework".
// Got it from: http://www.jera.com/techinfo/jtns/jtn002.html

#ifndef _BBB_TESTS_MINUNIT_H
#define _BBB_TESTS_MINUNIT_H

#define BBB_MU_ASSERT(message, test)	do { if ( !( test ) ) return message; } while ( 0 )
#define BBB_MU_RUN_TEST(test)			do { char* message = test(); _testsRun++; \
											if ( message ) return message; } while ( 0 )
#define DATA_DIR	"../../../test_data"

#define MAIN \
	int main() { \
		char*	result; \
	\
		result = _AllTests(); \
	\
		if ( result == 0 ) { \
			printf( "\nALL %d TESTS PASSED\n", _testsRun ); \
		} else { \
			printf( "\n*** TEST FAILED: %s\n", result ); \
		} \
	\
		return ( int ) ( result != 0 ); \
	}

#endif

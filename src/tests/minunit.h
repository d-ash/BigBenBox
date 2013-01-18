// Minimal unit testing "framework".
// Got it from: http://www.jera.com/techinfo/jtns/jtn002.html

#ifndef _BBB_TESTS_MINUNIT_H
#define _BBB_TESTS_MINUNIT_H

#define BBB_MU_ASSERT(message, test)	do { if ( !( test ) ) return message; } while ( 0 )
#define BBB_MU_RUN_TEST(test)			do { char* message = test(); _testsRun++; \
											if ( message ) return message; } while ( 0 )

#endif

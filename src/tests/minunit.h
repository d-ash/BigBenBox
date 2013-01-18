// Minimal unit testing "framework".
// Got it from: http://www.jera.com/techinfo/jtns/jtn002.html

#ifndef _BBB_MINUNIT_H
#define _BBB_MINUNIT_H

#define MU_ASSERT(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char* message = test(); tests_run++; \
	                                 if (message) return message; } while (0)
extern int tests_run;

#endif

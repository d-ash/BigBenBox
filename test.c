#include <stdio.h>

#include "minunit.h"
#include "tools.h"

int tests_run = 0;

static char* test_uint16_hash() {
	MU_ASSERT("uint16_hash() failed", uint16_hash("abcde", 5) == 0x1d63);
	return 0;
}

static char* test_strncmp() {
	// strncmp()'s third parameter must be (strlen() + 1)
	MU_ASSERT("strncmp() failed (1)", strncmp("abcdef", "abcde", 5) == 0);
	MU_ASSERT("strncmp() failed (2)", strncmp("abcdef", "abcde", 6) != 0);
	return 0;
}

// ================================================

static char* all_tests() {
	mu_run_test(test_uint16_hash);
	mu_run_test(test_strncmp);
	return 0;
}

int main(int argc, char* argv[]) {
	char* result = all_tests();
	if (result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return (int) (result != 0);
}

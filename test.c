#include <stdio.h>

#include "minunit.h"
#include "tools.h"

int tests_run = 0;

static char* test_md5() {
	unsigned char md_true[HASH_SUM_LENGTH] =
		{0xab,0x56,0xb4,0xd9,0x2b,0x40,0x71,0x3a,
		0xcc,0x5a,0xf8,0x99,0x85,0xd4,0xb7,0x86};
	unsigned char md[HASH_SUM_LENGTH];

	MD5("abcde", 5, md);
	MU_ASSERT("MD5() failed", memcmp(md, md_true, HASH_SUM_LENGTH) == 0);

	return 0;
}

static char* test_uint16_hash() {
	MU_ASSERT("uint16_hash() failed", uint16_hash("abcde", 5) == 0x1d63);
	return 0;
}

// ================================================

static char* all_tests() {
	mu_run_test(test_md5);
	mu_run_test(test_uint16_hash);
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

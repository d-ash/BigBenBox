#include <stdio.h>

#include "minunit.h"
#include "tools.h"
#include "snapshot.h"
#include "pack.h"

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

static char* test_ssentry_size() {
	//printf("sizeof(SSENTRY): %u\n", sizeof(SSENTRY));
	MU_ASSERT("sizeof(SSENTRY) is not appropriate for manual path aligment.", (sizeof(SSENTRY) % WORD_SIZE) == 0);
	return 0;
}

static char* test_snapshot_save_load() {
	SNAPSHOT ss;
	SNAPSHOT ss2;

#ifdef PLATFORM_WINDOWS
	take_snapshot("G:/English", &ss);
	//take_snapshot("C:/Windows", &ss);
#else
	take_snapshot("/home/d-ash/distr", &ss);
	//take_snapshot("/home/d-ash/2IOMEGA/", &ss);
#endif // PLATFORM_WINDOWS

	save_snapshot("_test_packfile", &ss);
	load_snapshot("_test_packfile", &ss2);
	MU_ASSERT("Restored snapshot differs from original", find_changes(&ss, &ss2) == 0);

	//unlink("_test_packfile");
	destroy_snapshot(&ss2);
	destroy_snapshot(&ss);

	return 0;
}

// ================================================

static char* all_tests() {
	mu_run_test(test_uint16_hash);
	mu_run_test(test_strncmp);
	mu_run_test(test_ssentry_size);
	mu_run_test(test_snapshot_save_load);
	return 0;
}

int main(int argc, char* argv[]) {
	char* result;
	
	result = all_tests();

	if (result == 0) {
		printf("\nALL TESTS PASSED\n");
	} else {
		printf("\nFAILED: %s\n", result);
	}
	printf("Tests run: %d\n", tests_run);

	return (int) (result != 0);
}

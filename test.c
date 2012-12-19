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

static char* test_sha256() {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned char ctrl[SHA256_DIGEST_LENGTH] =
		{0xcc, 0xfa, 0xf6, 0xdb, 0xae, 0x74, 0x8e, 0xa5,
		0x4e, 0x16, 0x32, 0x2b, 0xcd, 0x04, 0xeb, 0x13,
		0x75, 0x08, 0x3e, 0x65, 0x11, 0x28, 0xae, 0x0c,
		0xe0, 0x7d, 0xdb, 0x6d, 0xce, 0x4f, 0x6e, 0xeb};

	MU_ASSERT("Cannot calc SHA256 on a file", sha256_file("tmp0/sha.hash", hash) != 0);
	MU_ASSERT("SHA256 do not match", memcmp(hash, ctrl, SHA256_DIGEST_LENGTH) == 0);
	return 0;
}

static char* test_ssentry_size() {
	printf("sizeof(SSENTRY): %u\n", sizeof(SSENTRY));
	MU_ASSERT("sizeof(SSENTRY) is not appropriate for manual path aligment.", (sizeof(SSENTRY) % WORD_SIZE) == 0);
	return 0;
}

static char* test_snapshot_save_load() {
	SNAPSHOT ss1;
	SNAPSHOT ss2;

#ifdef PLATFORM_WINDOWS
	take_snapshot("G:\\English\\", &ss1);
	//take_snapshot("C:/Windows", &ss1);
#else
	take_snapshot("/home/d-ash/distr/", &ss1);
	//take_snapshot("/home/d-ash/2IOMEGA/", &ss1);
#endif // PLATFORM_WINDOWS

	save_snapshot("_test_packfile", &ss1);
	load_snapshot("_test_packfile", &ss2);

	MU_ASSERT("Different values of tf_path", strcmp(ss1.tf_path, ss2.tf_path) == 0);
	MU_ASSERT("Restored snapshot differs from original", find_changes(&ss1, &ss2) == 0);

	//unlink("_test_packfile");
	destroy_snapshot(&ss2);
	destroy_snapshot(&ss1);

	return 0;
}

// ================================================

static char* all_tests() {
	mu_run_test(test_uint16_hash);
	mu_run_test(test_strncmp);
	mu_run_test(test_sha256);
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

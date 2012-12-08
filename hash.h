#ifndef _HASH_H
#define _HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <openssl/md5.h>

uint16_t uint16_hash(unsigned char* buf, int len);

// Print the MD5 sum as hex-digits.
void print_md5_sum(unsigned char* md);

#endif	// _HASH_H

#pragma once

#if defined(UNIT_TEST)
#include "unit_test_rand.h"
#else
#include "pcg_variants.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "sub_t.h"
#include "mysecond.h"

#define MASK 0xfc00000000000000

//-----------------------------------------------------------------------------
// Encrypt
//-----------------------------------------------------------------------------
void encrypt_night(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt
//-----------------------------------------------------------------------------
void decrypt_night(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Anchor rotation based off the first 6 bits of the key
//-----------------------------------------------------------------------------
inline uint64_t rotr64 (uint64_t n, unsigned int c){
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}


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
#include "sub_t.h"

#define WORD_SIZE 8
#define MASK 0x3F

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
// Anchor rotation based off the last 6 bits of the key
//-----------------------------------------------------------------------------
inline uint64_t rotr64 (uint64_t n, unsigned int c){
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}


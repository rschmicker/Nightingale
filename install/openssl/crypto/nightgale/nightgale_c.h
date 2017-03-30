#ifndef HEADER_NIGHTGALE_H
#define HEADER_NIGHTGALE_H

#include "pcg_variants.h"
#include "sub_t.h"

#define WORD_SIZE 8
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
// Anchor rotation based off the last 6 bits of the key
//-----------------------------------------------------------------------------
inline uint64_t rotr64 (uint64_t n, unsigned int c){
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}

#endif


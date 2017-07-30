#pragma once

#if defined(UNIT_TEST)
#include "unit_test_rand.h"
#else
#include "pcg_variants.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <immintrin.h>
#include <stdbool.h>
#include "sub_t.h"
#include "mysecond.h"

#define VEC_SIZE 32
#define MASK 0xfc00000000000000

static const unsigned char low_bytes[] =
            {
               0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
               0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
               0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
               0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0
           };

static const unsigned char high_bytes[] =
            {
                0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
                0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
                0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
                0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F
            };

static const int multi_factor_right[] = {4, 4, 4, 4, 4, 4, 4, 4};

typedef struct{
    uint64_t *in;
    uint64_t *out;
    size_t td_word_count;
    SUB *s;
    size_t stream_num;
}thread_data;

typedef struct{
    __m256i *in;
    __m256i *out;
    size_t td_word_count;
    SUB_V *s;
    size_t stream_num;
}thread_vec_data;

//-----------------------------------------------------------------------------
// Encrypt CBC like
//-----------------------------------------------------------------------------
void encrypt_night(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt CBC like
//-----------------------------------------------------------------------------
void decrypt_night(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Encrypt in parallel
//-----------------------------------------------------------------------------
void encrypt_night_p(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Encrypt thread subroutine
//-----------------------------------------------------------------------------
void* encrypt_threaded(void *t);

//-----------------------------------------------------------------------------
// Decrypt in parallel
//-----------------------------------------------------------------------------
void decrypt_night_p(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt thread subroutine
//-----------------------------------------------------------------------------
void* decrypt_threaded(void *t);

//-----------------------------------------------------------------------------
// Encrypt in parallel - vectorized
//-----------------------------------------------------------------------------
void encrypt_night_pv(SUB_V *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Encrypt thread subroutine - vectorized
//-----------------------------------------------------------------------------
void* encrypt_threaded_v(void *t);

//-----------------------------------------------------------------------------
// Decrypt in parallel - vectorized
//-----------------------------------------------------------------------------
void decrypt_night_pv(SUB_V *s, size_t len, const unsigned char *in,
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt thread subroutine - vectorized
//-----------------------------------------------------------------------------
void* decrypt_threaded_v(void *t);

//-----------------------------------------------------------------------------
// Round down to the previous power of 2 of any given number
//-----------------------------------------------------------------------------
inline size_t round_power_down(size_t x)
{
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x - (x >> 1);
}

//-----------------------------------------------------------------------------
// Anchor rotation based off the first 6 bits of the key
//-----------------------------------------------------------------------------
inline uint64_t rotr64 (uint64_t n, unsigned int c){
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}

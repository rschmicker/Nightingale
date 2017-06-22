#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sub_t.h"
#include "mysecond.h"

typedef struct{
    uint64_t *in;
    uint64_t *out;
    size_t td_word_count;
    SUB *s;
    size_t stream_num;
}thread_data;

//-----------------------------------------------------------------------------
// Encrypt
//-----------------------------------------------------------------------------
void encrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Encrypt thread subroutine
//-----------------------------------------------------------------------------
void* encrypt_threaded(void *t);

//-----------------------------------------------------------------------------
// Decrypt
//-----------------------------------------------------------------------------
void decrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt thread subroutine
//-----------------------------------------------------------------------------
void* decrypt_threaded(void *t);

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

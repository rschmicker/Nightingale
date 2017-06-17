#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sub_t.h"
#include "mysecond.h"

typedef struct{
    uint64_t *in;
    uint64_t *out;
    size_t td_word_count;
    SUB *s;
}thread_data;

//-----------------------------------------------------------------------------
// Encrypt
//-----------------------------------------------------------------------------
void encrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Encrypt thread subroutine
//-----------------------------------------------------------------------------
void* encrypt(void *t);

//-----------------------------------------------------------------------------
// Decrypt
//-----------------------------------------------------------------------------
void decrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out);

//-----------------------------------------------------------------------------
// Decrypt thread subroutine
//-----------------------------------------------------------------------------
void* decrypt(void *t);


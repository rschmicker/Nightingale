/*
 * Copyright 1999-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HEADER_NIGHT_H
# define HEADER_NIGHT_H

#define SEED_SIZE 16
#define SUB_SIZE 256
#define WORD_SIZE 8

#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

typedef struct{
    char*           filename;
    char*           outputname;
    unsigned char   hash[SHA512_DIGEST_LENGTH];
    unsigned char   digest[SHA256_DIGEST_LENGTH];
    unsigned char   seed1[SEED_SIZE];
    unsigned char   seed2[SEED_SIZE];
    unsigned char   seed3[SEED_SIZE];
    unsigned char   seed4[SEED_SIZE];
    uint64_t        sub_rands[SUB_SIZE];
    unsigned char   sub[SUB_SIZE];
    unsigned char   reverse_sub[SUB_SIZE];
}SUB;

typedef struct{
        unsigned int index;
        unsigned int value;
}indexes;

void nightgale_enc_set_key(SUB *s);
void nightgale_dec_set_key(SUB *s);

void encrypt_night(SUB *s, size_t len, const unsigned char *in,
                    unsigned char *out);


void decrypt_night(SUB *s, size_t len, const unsigned char *in,
                    unsigned char *out);

#endif

#ifdef __cplusplus
}
#endif


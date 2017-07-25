#pragma once

#if defined(UNIT_TEST)
#include "unit_test_rand.h"
#else
#include "pcg_variants.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#define SEED_SIZE 16
#define SUB_SIZE 256
#define VECTOR_SUB_SIZE 64
#define RSA_KEY "private.pem"
#define WORD_SIZE 8
#define VECTOR_WORD_SIZE 32
#define VECTOR_RAND_SIZE 16

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

// Fix Sub table for vectorize
// generate 4 0-15 arrays of values from 0-15 in random order
// concat to make two pairs
// use that for sub_v 1 and 2 and inverse for reverse_sub

typedef struct{
    char*           filename;
    char*           outputname;
    unsigned char   hash[SHA512_DIGEST_LENGTH];
    unsigned char   digest[SHA256_DIGEST_LENGTH];
    unsigned char   seed1[SEED_SIZE];
    unsigned char   seed2[SEED_SIZE];
    unsigned char   seed3[SEED_SIZE];
    unsigned char   seed4[SEED_SIZE];
    uint64_t        sub_rands_1[VECTOR_RAND_SIZE];
    uint64_t        sub_rands_2[VECTOR_RAND_SIZE];
    uint64_t        sub_rands_3[VECTOR_RAND_SIZE];
    uint64_t        sub_rands_4[VECTOR_RAND_SIZE];
    unsigned char   sub_v_low[VECTOR_WORD_SIZE];
    unsigned char   sub_v_high[VECTOR_WORD_SIZE];
    unsigned char   r_sub_v_low[VECTOR_WORD_SIZE];
    unsigned char   r_sub_v_high[VECTOR_WORD_SIZE];
}SUB_V;

typedef struct{
        unsigned int index;
        uint64_t value;
}indexes;

//-----------------------------------------------------------------------------
// Set the sub table for nightgale
//-----------------------------------------------------------------------------
void nightgale_enc_set_key(SUB *s);
void nightgale_dec_set_key(SUB *s);
void nightgale_enc_set_key_v(SUB_V *s);
void nightgale_dec_set_key_v(SUB_V *s);

//-----------------------------------------------------------------------------
// Get the file length with fseek()
//-----------------------------------------------------------------------------
long unsigned int get_file_length(FILE *fp);

//-----------------------------------------------------------------------------
// Free resources for RSA key creation
//-----------------------------------------------------------------------------
void free_r(BIO *bp_private, RSA *r, BIGNUM *bne);

//-----------------------------------------------------------------------------
// Read binary key file
//-----------------------------------------------------------------------------
void read_key(SUB *s);

//-----------------------------------------------------------------------------
// Output key to screen and binary file
//-----------------------------------------------------------------------------
void write_key(SUB *s);

//-----------------------------------------------------------------------------
// Compare function for qsort
//-----------------------------------------------------------------------------
int cmp(const void * elem1, const void * elem2);

//-----------------------------------------------------------------------------
// Shuffle the 1 - 256 (64 for vectorised) values using a Knuth shuffle
//-----------------------------------------------------------------------------
void shuffle(SUB *s);
void shuffle_v(SUB_V *s);

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SUB *s);
void generate_rands_v(SUB_V *s);

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SUB *s);
void generate_seeds_v(SUB_V *s);

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SUB *s, const char* key_file);
void generate_hash_v(SUB_V *s, const char* key_file);

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key();

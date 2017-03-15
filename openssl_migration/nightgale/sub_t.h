#ifndef HEADER_SUB_H
#define HEADER_SUB_G

#include "pcg_variants.h"
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
#define RSA_KEY "private.pem"

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

//-----------------------------------------------------------------------------
// Set the sub table for nightgale
//-----------------------------------------------------------------------------
void nightgale_enc_set_key(SUB *s);
void nightgale_dec_set_key(SUB *s);

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
// Shuffle the 1 - 256 values using a Knuth shuffle
//-----------------------------------------------------------------------------
void shuffle(SUB *s);

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SUB *s);

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SUB *s);

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SUB *s, const char* key_file);

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key();

#endif


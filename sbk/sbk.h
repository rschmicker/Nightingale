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
#include "pcg_variants.h"

#define SEED_SIZE 16
#define TRANSPOSE_SIZE 32
#define SBK_SIZE 128
#define RAND_SIZE 64
#define SEED_SIZE_MULTIPLYER 8
#define PNRG_SIZE 1024
#define RSA_KEY "private.pem"
#define SBK_KEY "key.sbk"

typedef struct{
    char*           filename;
    char*           outputname;
    unsigned char   hash[SHA512_DIGEST_LENGTH];
    unsigned char   seed1[SEED_SIZE];
    unsigned char   seed2[SEED_SIZE];
    unsigned char   seed3[SEED_SIZE];
    unsigned char   seed4[SEED_SIZE];
    unsigned int 	transpose1[TRANSPOSE_SIZE];
    unsigned int 	transpose2[TRANSPOSE_SIZE];
    unsigned int 	transpose3[TRANSPOSE_SIZE];
    unsigned int 	transpose4[TRANSPOSE_SIZE];
    unsigned int 	transposed[SBK_SIZE];
    unsigned int    sbk[SBK_SIZE];
}SBK;

typedef struct{
        unsigned int index;
        unsigned int value;
}indexes;

//-----------------------------------------------------------------------------
// Free resources for RSA key creation
//-----------------------------------------------------------------------------
void free_r(BIO *bp_private, RSA *r, BIGNUM *bne);

//-----------------------------------------------------------------------------
// Read binary key file
//-----------------------------------------------------------------------------
void read_key(SBK *s);

//-----------------------------------------------------------------------------
// Output key to screen and binary file
//-----------------------------------------------------------------------------
void write_key(SBK *s);

//-----------------------------------------------------------------------------
// Compare function for qsort
//-----------------------------------------------------------------------------
int cmp(const void * elem1, const void * elem2);

//-----------------------------------------------------------------------------
// Shuffle the 1 - 128 values using a Knuth shuffle
//-----------------------------------------------------------------------------
void shuffle(SBK *s);

//-----------------------------------------------------------------------------
// Transpose the 4 arrays of random numbers
//-----------------------------------------------------------------------------
void transpose(SBK *s);

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SBK *s);

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SBK *s);

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SBK *s);

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key(SBK *s);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#define SEED_SIZE 16
#define SBK_SIZE 128

typedef struct{
    char*           filename;
    char*           outputname;
    unsigned char   hash[SHA512_DIGEST_LENGTH];
    unsigned char   seed1[SEED_SIZE];
    unsigned char   seed2[SEED_SIZE];
    unsigned char   seed3[SEED_SIZE];
    unsigned char   seed4[SEED_SIZE];
    unsigned short  sbk[SBK_SIZE];
}SBK;

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

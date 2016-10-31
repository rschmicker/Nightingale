#include <stdio.h>
#include <stdlib.h>
#include "stringinfo.h"
#include "sub_t.h"
#include "pcg_variants.h"
#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"
#define SEED_CAPACITY 4

typedef struct{
    uint64_t anchor;
    uint64_t keys[SEED_CAPACITY];
}NIGHT;

//-----------------------------------------------------------------------------
// Encrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, const char* file);

//-----------------------------------------------------------------------------
// Decrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void decrypt_file(NIGHT *n);

//-----------------------------------------------------------------------------
// Write pacc to file
//-----------------------------------------------------------------------------
void write_pacc(NIGHT *n);

//-----------------------------------------------------------------------------
// Read pacc from file
//-----------------------------------------------------------------------------
void read_pacc(NIGHT *n);

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "sub_t.h"
#include "pcg_variants.h"
#include "mysecond.h"
#include "sub_t.h"

#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"
//#define KEY "private.pem"
#define NIGHT_KEY "night.key"
#define WORD_SIZE 8

typedef struct{
    int			file_char_length;
    int			pad;
    int			word_count;
}NIGHT;

//-----------------------------------------------------------------------------
// Get the file length with fseek()
//-----------------------------------------------------------------------------
long unsigned int get_file_length(FILE *fp);

//-----------------------------------------------------------------------------
// Encrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, SUB *s, const char* file);

//-----------------------------------------------------------------------------
// Encrypt
//-----------------------------------------------------------------------------
unsigned char *encrypt(NIGHT *n, SUB *s, unsigned char *message);

//-----------------------------------------------------------------------------
// Decrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void decrypt_file(const char* cipher_text, const char* night_key_file, const char* rsa_key_file);

//-----------------------------------------------------------------------------
// Decrypt
//-----------------------------------------------------------------------------
unsigned char *decrypt(NIGHT *n, SUB *s, uint64_t *enc_message);

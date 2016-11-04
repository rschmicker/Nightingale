#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "stringinfo.h"
#include "sub_t.h"
#include "pcg_variants.h"

#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"
#define KEY "night.key"
#define WORD_SIZE 8

typedef struct{
    uint64_t 	anchor;
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
// Decrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void decrypt_file(const char* cipher_text, const char* key_file);

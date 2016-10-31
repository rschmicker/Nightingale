#include <stdio.h>
#include <stdlib.h>
#include "stringinfo.h"
#define PACC_SIZE 64
#define MAP_SIZE 255
#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"
#define PACC_KEY_NAME "assignment.pacc"

typedef struct{
    unsigned char map1b[MAP_SIZE];
    unsigned char map1c[MAP_SIZE];
	int file_char_length;
}PACC;

//-----------------------------------------------------------------------------
// Fill the pacc map
//-----------------------------------------------------------------------------
void encode(PACC *p);

//-----------------------------------------------------------------------------
// Encrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void encrypt_file(PACC *p, const char* file);

//-----------------------------------------------------------------------------
// Decrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void decrypt_file(PACC *p);

//-----------------------------------------------------------------------------
// Write pacc to file
//-----------------------------------------------------------------------------
void write_pacc(PACC *p);

//-----------------------------------------------------------------------------
// Read pacc from file
//-----------------------------------------------------------------------------
void read_pacc(PACC *p);

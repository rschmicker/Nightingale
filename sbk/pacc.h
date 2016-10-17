#include <stdio.h>
#include <stdlib.h>
#define PACC_SIZE 64
#define MAP_SIZE 255
#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"

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

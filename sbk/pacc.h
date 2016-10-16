#include <stdio.h>
#include <stdlib.h>
#define PACC_SIZE 64
#define MAP_SIZE 255
#define E_FILE "encrypted_file.txt"
#define D_FILE "decrypted_file.txt"

// typedef struct{
// 	unsigned char ascii_hex;
// 	unsigned char pacc;
// }MAP;

typedef struct{
    //MAP map1[PACC_SIZE];
    unsigned char map1[MAP_SIZE];
	int file_char_length;
}PACC;

//-----------------------------------------------------------------------------
// Fill the pacc map
//-----------------------------------------------------------------------------
void encode(PACC *p);

//-----------------------------------------------------------------------------
// Print the map
//-----------------------------------------------------------------------------
void print_encoded_map(PACC *p);

//-----------------------------------------------------------------------------
// Decode the pacc map
//-----------------------------------------------------------------------------
void print_decoded_map(PACC *p);

//-----------------------------------------------------------------------------
// Helper function for print_encoded_map() & print_decoded_map
//-----------------------------------------------------------------------------
void get_hex(int n, unsigned char* hex);

//-----------------------------------------------------------------------------
// Helper function for print_encoded_map() & print_decoded_map
//-----------------------------------------------------------------------------
int get_ascii(unsigned char c);

//-----------------------------------------------------------------------------
// Encrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void encrypt_file(PACC *p, const char* file);

//-----------------------------------------------------------------------------
// Decrypt the file using the pacc lookup table
//-----------------------------------------------------------------------------
void decrypt_file(PACC *p);

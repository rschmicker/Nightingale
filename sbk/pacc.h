#include <stdio.h>
#define PACC_SIZE 64

typedef struct{
	unsigned char ascii_hex;
	unsigned char pacc;
}MAP;

typedef struct{
    MAP map1[PACC_SIZE];
}PACC;

//-----------------------------------------------------------------------------
// Fill the pacc map
//-----------------------------------------------------------------------------
void encode(PACC *p);

//-----------------------------------------------------------------------------
// Print the map
//-----------------------------------------------------------------------------
void print_encoded_map(PACC *p);

<<<<<<< HEAD
//-----------------------------------------------------------------------------
// Decode the pacc map
//-----------------------------------------------------------------------------
void print_decoded_map(PACC *p);

//-----------------------------------------------------------------------------
// Helper function for print_encoded_map() & print_decoded_map
//-----------------------------------------------------------------------------
void get_hex(int n, char* hex);

//-----------------------------------------------------------------------------
// Helper function for print_encoded_map() & print_decoded_map
//-----------------------------------------------------------------------------
int get_ascii(char c);
=======
void get_hex(int n, unsigned char* hex);

int get_ascii(unsigned char c);
>>>>>>> 4ceb0b8e9da582d958159d2513fee3b9a84e1fdc

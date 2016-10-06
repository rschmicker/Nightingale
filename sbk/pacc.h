/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PACC_SIZE 64

typedef struct{
	char ascii_hex;
	char pacc;
}MAP;

typedef struct{
    MAP map1[PACC_SIZE];
}PACC;

void fill_ascii_to_pacc_map(PACC *p);

void ascii_hex_to_pacc(PACC *p);

void get_hex(int n, char* hex);

int get_ascii(char c);
*/

#include <stdio.h>
#define PACC_SIZE 64

typedef struct{
	unsigned char ascii_hex;
	unsigned char pacc;
}MAP;

typedef struct{
    MAP map1[PACC_SIZE];
}PACC;

void fill_ascii_to_pacc_map(PACC *p);

void ascii_hex_to_pacc(PACC *p);

void get_hex(int n, unsigned char* hex);

int get_ascii(unsigned char c);
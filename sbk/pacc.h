#include <stdio.h>
#include "dict.h"
#define PACC_SIZE 64

typedef struct{
	struct nlist *np;
    unsigned char arrayChar1[PACC_SIZE];
    unsigned char arrayByt1[PACC_SIZE];
}PACC;

void fill_ascii_to_pacc_map(PACC *p);

void ascii_hex_to_pacc(PACC *p);

void get_hex(int n, char* hex);

int get_ascii(char c);

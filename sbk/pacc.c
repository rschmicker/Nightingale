#include "pacc.h"

void fill_ascii_to_pacc_map(PACC *p){
    const unsigned char tempChar1[PACC_SIZE] = {
        '\0', ' ', ',', '.',
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R',
        'S', 'T', 'U', 'V',
        'W', 'Y', 'a', 'b',
        'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r',
        's', 't', 'u', 'v',
        'w', 'x', 'y', 'z'
    };

    const unsigned char tempBytes1[PACC_SIZE] = {
        0x0F, 0x17, 0x1B, 0x1D,
        0x1E, 0x27, 0x2B, 0x2D,
        0x2E, 0x33, 0x35, 0x36,
        0x39, 0x3A, 0x3C, 0x47,
        0x4B, 0x4D, 0x4E, 0x53,
        0x55, 0x56, 0x59, 0x5A,
        0x5C, 0x63, 0x65, 0x66,
        0x69, 0x6A, 0x6C, 0x71,
        0x72, 0x74, 0x78, 0x87,
        0x8B, 0x8D, 0x8E, 0x93,
        0x95, 0x96, 0x99, 0x9A,
        0x9C, 0xA3, 0xA5, 0xA6,
        0xA9, 0xAA, 0xAC, 0xB1,
        0xB2, 0xB4, 0xB8, 0xC3,
        0xC5, 0xC6, 0xC9, 0xCA,
        0xCC, 0xD1, 0xD2, 0xD4
    };

    for(int i = 0; i < PACC_SIZE; i++){
        p->arrayChar1[i] = tempChar1[i];
        p->arrayByt1[i] = tempBytes1[i];
    }
}

void ascii_hex_to_pacc(PACC *p){
    printf("Hex Encoding Map for Array 1 (ASCII to PACC):\n");
    for(int i = 0; i < PACC_SIZE; i++){
        if(i % 8 == 0 && i != 0) printf("\n");
        char PACC_hex[2];
        char ascii_hex[2];
        char* ph = PACC_hex;
        char* ah = ascii_hex;
        get_hex((int)p->arrayByt1[i], ph);
        get_hex(get_ascii(p->arrayChar1[i]), ah);
        printf("%c%c -> %c%c, ", ah[0], ah[1], ph[0], ph[1]);

    }
    printf("\n");
}

void get_hex(int n, char* hex){
    sprintf(hex, "%02x", n);
}

int get_ascii(char c){
    return (int)c;
}

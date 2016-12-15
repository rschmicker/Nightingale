#include <assert.h> 
#include "sub_t.h"
#include "nightgale_c.h"

#define UT_RSA_KEY "harlen.pem"

//-----------------------------------------------------------------------------
void unit_test(){

    // Message to test
    unsigned char* message = "hello my name is paul!";

    // Create sub table for testing using "harlen.pem"
    SUB s;
    printf("Gen hash...\n");
    generate_hash(&s, UT_RSA_KEY);
    printf("Gen seeds...\n");
    generate_seeds(&s);
    printf("Gen rands...\n");
    generate_rands(&s);
    printf("Shuffle...\n");
    shuffle(&s);

    // Create night struct for storing 
    // length and pading of message
    NIGHT n;

    // Pad length of message before encrypt
    n.length = strlen(message);
    printf("Length: %d\n", n.length);
    n.word_count = n.length / WORD_SIZE;
    if(n.length % WORD_SIZE != 0) ++n.word_count;
    n.pad = WORD_SIZE - n.length % WORD_SIZE;
    if(n.pad == 8) n.pad = 0;
    printf("pad: %d\n", n.pad);

    // Create buffer for message if padding is necessary
    unsigned char* buffer = calloc(sizeof(unsigned char), n.length + n.pad);
    memcpy(buffer, message, n.length);


    // encrypt
    unsigned char* enc_buffer = encrypt(&n, &s, buffer);

    unsigned char* dec_buffer = decrypt(&n, &s, enc_buffer);

    int check = memcmp( buffer, dec_buffer, n.length );

    assert(check == 0);
    printf("Passed first message!\n");

    free(buffer);
    free(enc_buffer);
    free(dec_buffer);
}

int main(){

	unit_test();

	return 0;
}

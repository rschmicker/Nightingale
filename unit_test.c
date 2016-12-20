#include <assert.h> 
#include "sub_t.h"
#include "nightgale_c.h"

#define UT_RSA_KEY "harlen.pem"

//-----------------------------------------------------------------------------
void unit_test(){
    // Messages to test

    // Message 1
    //=========================================================================
    unsigned char buffer1[] = {50,46,55,49,56,50,56,49,56,50,56,52,53,57,
                                    48,52,53,50,51,53,51,54,48};
    unsigned char* message1 = buffer1;
    int message1_length = 23;

    unsigned char enc_buffer1[] = {7, 236, 207, 78, 222, 33, 98, 119, 60, 
        27, 179, 249, 114, 244, 220, 138, 242, 96, 34, 242, 209, 83, 9, 38};
    unsigned char* enc_message1 = enc_buffer1;

    // Message 2
    //=========================================================================
    unsigned char buffer2[] = {73,116,39,115,32,108,105,107,101,32,100,
        233,106,224,32,118,117,32,97,108,108,32,111,118,101,114,32,97,
        103,97,105,110,46};
    unsigned char* message2 = buffer2;
    int message2_length = 33;

    unsigned char enc_buffer2[] = {166, 119, 241, 34, 251, 130, 124, 37, 133,
        47, 108, 53, 183, 195, 188, 119, 195, 215, 107, 84, 150, 52, 65, 199, 
        83, 100, 144, 116, 63, 155, 250, 146, 151, 174, 0, 246, 19, 45, 246, 157};
    unsigned char* enc_message2 = enc_buffer2;


    // Message 3
    //=========================================================================
    unsigned char buffer3[] = {72,111,119,32,109,117,99,104,32,119,111,
        111,100,32,119,111,117,108,100,32,97,32,119,111,111,100,99,104,
        117,99,107,32,99,104,117,99,107,32,105,102,32,97,32,119,111,111,
        100,99,104,117,99,107,32,99,111,117,108,100,32,99,104,117,99,107,
        32,119,111,111,100,63,63,63};
    unsigned char* message3 = buffer3;
    int message3_length = 72;

    unsigned char enc_buffer3[] = {117, 246, 235, 180, 37, 179, 240, 192, 78, 
        223, 120, 225, 14, 155, 162, 157, 224, 39, 86, 214, 230, 184, 178, 168, 
        90, 53, 183, 96, 146, 242, 55, 162, 210, 76, 129, 171, 108, 195, 163, 234, 
        191, 76, 223, 77, 163, 192, 12, 224, 73, 86, 186, 162, 22, 151, 209, 118, 
        192, 71, 110, 255, 25, 203, 123, 6, 30, 216, 84, 10, 47, 153, 137, 149};
    unsigned char* enc_message3 = enc_buffer3;

    // Create sub table for testing 
    SUB s;
    int temp[] = {93, 86, 57, 183, 229, 135, 94, 73, 6, 41, 192, 19, 75, 24, 137, 
        29, 60, 196, 158, 78, 243, 122, 58, 116, 84, 195, 67, 146, 30, 38, 
        256, 3, 99, 106, 214, 244, 33, 160, 18, 211, 224, 246, 228, 21, 190, 
        28, 31, 50, 132, 108, 23, 182, 168, 254, 110, 236, 210, 207, 10, 205, 
        231, 238, 163, 103, 185, 148, 204, 83, 63, 250, 200, 89, 131, 140, 162,
        217, 5, 189, 197, 251, 77, 186, 215, 199, 240, 252, 25, 230, 118, 212, 
        247, 81, 26, 2, 79, 80, 82, 178, 176, 96, 180, 40, 174, 235, 136, 72, 
        92, 171, 15, 130, 139, 245, 120, 97, 66, 237, 71, 202, 123, 177, 68, 
        114, 156, 119, 241, 179, 14, 129, 126, 115, 209, 150, 133, 20, 184, 
        95, 216, 105, 152, 226, 56, 166, 232, 11, 70, 44, 109, 203, 4, 218, 
        198, 159, 188, 249, 155, 53, 102, 98, 149, 1, 12, 49, 100, 143, 34, 
        125, 175, 127, 32, 37, 74, 85, 48, 169, 253, 76, 90, 35, 225, 164, 
        134, 128, 157, 47, 36, 141, 65, 153, 45, 112, 117, 91, 227, 165, 172, 
        54, 8, 101, 170, 223, 233, 138, 222, 147, 220, 64, 208, 167, 51, 27, 
        121, 22, 151, 213, 173, 142, 43, 124, 7, 104, 87, 234, 16, 61, 239, 39,
        191, 194, 201, 107, 193, 59, 248, 144, 42, 55, 242, 145, 69, 221, 111,
        13, 255, 161, 88, 219, 187, 52, 113, 62, 17, 181, 206, 154, 46, 9};

    for(int i = 0; i < SUB_SIZE; i++){
        s.sub[i] = temp[i];
        s.reverse_sub[(unsigned char)s.sub[i]] = i;
    }

    // Create night struct for storing 
    // length and pading of message
    NIGHT n;

    // Storage of buffer values
    unsigned char* buffer;
    unsigned char* enc_buffer;
    unsigned char* dec_buffer;

    // Assert value
    int check;

    // Encrypt and decrypt message 1
    //=========================================================================

    // Pad length of message before encrypt
    n.length = message1_length;
    printf("Length: %d\n", n.length);
    n.word_count = n.length / WORD_SIZE;
    if(n.length % WORD_SIZE != 0) ++n.word_count;
    printf("Word Count: %d\n", n.word_count);
    n.pad = WORD_SIZE - n.length % WORD_SIZE;
    if(n.pad == 8) n.pad = 0;
    printf("pad: %d\n", n.pad);

    // Create buffer for message if padding is necessary
    buffer = calloc(sizeof(unsigned char), n.length + n.pad);
    memcpy(buffer, message1, n.length);

    printf("buffer: %s\n", buffer);

    // encrypt
    enc_buffer = encrypt(&n, &s, buffer);

    printf("length=%d, padlen=%d\n", n.length, n.pad);
    printf("Encrypted buffer:\n");
    for (int i=0; i<24; i++) {
	printf("%d ",enc_buffer[i]);
    }
    printf("\n");
    printf("Check buffer:\n");
    for (int i=0; i<24; i++) {
	printf("%d ",enc_message1[i]);
    }
    printf("\n");
    printf("\n");
    printf("\n");
    check = memcmp(enc_buffer, enc_message1, n.length + n.pad); assert(check == 0);

    dec_buffer = decrypt(&n, &s, enc_buffer);

    printf("dec_buffer: %s\n", dec_buffer);

    check = memcmp( buffer, dec_buffer, n.length );      assert(check == 0);

    free(buffer);
    free(enc_buffer);
    free(dec_buffer);

    //=========================================================================
    // Encrypt and decrypt message 2
    //=========================================================================

    // Pad length of message before encrypt
    n.length = message2_length;
    printf("Length: %d\n", n.length);
    n.word_count = n.length / WORD_SIZE;
    if(n.length % WORD_SIZE != 0) ++n.word_count;
    n.pad = WORD_SIZE - n.length % WORD_SIZE;
    if(n.pad == 8) n.pad = 0;
    printf("pad: %d\n", n.pad);

    // Create buffer for message if padding is necessary
    buffer = calloc(sizeof(unsigned char), n.length + n.pad);
    memcpy(buffer, message2, n.length);


    // encrypt
    enc_buffer = encrypt(&n, &s, buffer);

    check = memcmp(enc_buffer, enc_buffer2,  n.length + n.pad);   assert(check == 0);

    dec_buffer = decrypt(&n, &s, enc_buffer);

    check = memcmp( buffer, dec_buffer, n.length );              assert(check == 0);

    free(buffer);
    free(enc_buffer);
    free(dec_buffer);

    //=========================================================================
    // Encrypt and decrypt message 3
    //=========================================================================

    // Pad length of message before encrypt
    n.length = message3_length;
    printf("Length: %d\n", n.length);
    n.word_count = n.length / WORD_SIZE;
    if(n.length % WORD_SIZE != 0) ++n.word_count;
    n.pad = WORD_SIZE - n.length % WORD_SIZE;
    if(n.pad == 8) n.pad = 0;
    printf("pad: %d\n", n.pad);

    // Create buffer for message if padding is necessary
    buffer = calloc(sizeof(unsigned char), n.length + n.pad);
    memcpy(buffer, message3, n.length);


    // encrypt
    enc_buffer = encrypt(&n, &s, buffer);

    check = memcmp(enc_buffer, enc_buffer3, n.length);   assert(check == 0);

    dec_buffer = decrypt(&n, &s, enc_buffer);

    check = memcmp( buffer, dec_buffer, n.length );              assert(check == 0);

    free(buffer);
    free(enc_buffer);
    free(dec_buffer);
}

int main(){

	unit_test();

	return 0;
}

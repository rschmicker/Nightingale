#include "unit_test.h"

#include "sub_t.h"
#include "nightgale_c.h"

#define UT_RSA_KEY "harlen.pem"

//-----------------------------------------------------------------------------
void unit_test(){

    // Messages to test
    const char* file1 = "./testfiles/message1.txt";
    const char* encfile1 = "./testfiles/enc_message1.txt";
    const char* decfile1 = "./testfiles/dec_message1.txt";

    const char* file2 = "./testfiles/message2.txt";
    const char* encfile2 = "./testfiles/enc_message2.txt";
    const char* decfile2 = "./testfiles/dec_message2.txt";

    const char* file3 = "./testfiles/message3.txt";
    const char* encfile3 = "./testfiles/enc_message3.txt";
    const char* decfile3 = "./testfiles/dec_message3.txt";

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

    // encrypt
    encrypt_file(&n, &s, file1, encfile1);    
    decrypt_file(encfile1, decfile1, NIGHT_KEY, UT_RSA_KEY);

    encrypt_file(&n, &s, file2, encfile2);
    decrypt_file(encfile2, decfile2, NIGHT_KEY, UT_RSA_KEY);

    encrypt_file(&n, &s, file3, encfile3);
    decrypt_file(encfile3, decfile3, NIGHT_KEY, UT_RSA_KEY);
    
}

int main(){

	unit_test();

	return 0;
}

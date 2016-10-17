#include "sbk.h"
#include "pacc.h"

int main(int argc, char *argv[]){
    const char* f;
    if( argc == 2 ) {
        f = argv[1];
    }
    else {
        printf("Please include file to encrypt.\n");
    }
    // SBK s;
    // s.filename = "private.pem";
    // s.outputname = "key.sbk";
    // generate_key(&s);
    // generate_hash(&s);
    // generate_seeds(&s);
    // generate_rands(&s);
    // transpose(&s);
    // shuffle(&s);
    // write_key(&s);
    // read_key(&s);

    PACC p;
    encode(&p);
    encrypt_file(&p, f);
    decrypt_file(&p);
    return 0;
}

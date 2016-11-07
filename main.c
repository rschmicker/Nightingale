#include "sub_t.h"
#include "nightgale_c.h"
#include "mysecond.h"

int main(int argc, char *argv[]){
    const char* f;
    if( argc == 2 ) {
        f = argv[1];
    }
    else {
        perror("Please include file to encrypt.\n");
        exit(EXIT_FAILURE);
    }
    SUB s;
    printf("Gen key...\n");
    generate_key(&s);
    printf("Gen hash...\n");
    generate_hash(&s);
    printf("Gen seeds...\n");
    generate_seeds(&s);
    printf("Gen rands...\n");
    generate_rands(&s);
    printf("Shuffle...\n");
    shuffle(&s);

    NIGHT n;
    
    encrypt_file(&n, &s, f);
    
    decrypt_file(E_FILE, KEY);

    return 0;
}

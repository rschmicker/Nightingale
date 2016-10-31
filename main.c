#include "sub_t.h"

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
    generate_key(&s);
    generate_hash(&s);
    generate_seeds(&s);
    generate_rands(&s);
    shuffle(&s);
    write_key(&s);
    read_key(&s);

    return 0;
}

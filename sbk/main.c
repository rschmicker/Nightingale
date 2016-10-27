#include "pcg_variants.h"
#include "sbk.h"
#include "pacc.h"

int main(int argc, char *argv[]){
    const char* f;
    if( argc == 2 ) {
        f = argv[1];
    }
    else {
        perror("Please include file to encrypt.\n");
        exit(EXIT_FAILURE);
    }
    SBK s;
    generate_key(&s);
    generate_hash(&s);
    generate_seeds(&s);
    generate_rands(&s);
    transpose(&s);
    shuffle(&s);
    write_key(&s);
    read_key(&s);

    PACC p;
    encode(&p);
    encrypt_file(&p, f);
    decrypt_file(&p);
    write_pacc(&p);
    read_pacc(&p);

    printf("==============================================");
    printf("\nI/O of PACC.\n");
    printf("==============================================");

    encode(&p);
    encrypt_file(&p, f);
    decrypt_file(&p);

    pcg32_random_t rng, rng2;
    int some_constant = 100000;
    uint64_t seed = 642364365436;

    pcg32_srandom_r(&rng, time(NULL), (intptr_t)&some_constant);
    pcg32_srandom_r(&rng2, seed, some_constant);

    uint32_t rand = pcg32_random_r(&rng);
    printf("Time Test:\t%u\n", rand);

    uint32_t rand_seed = pcg32_random_r(&rng2);
    printf("Seed Test:\t%u\n", rand_seed);


    return 0;
}

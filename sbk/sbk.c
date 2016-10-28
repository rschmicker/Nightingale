#include "sbk.h"

//-----------------------------------------------------------------------------
// Free resources for RSA key creation
//-----------------------------------------------------------------------------
void free_r(BIO *bp_private, RSA *r, BIGNUM *bne){
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);
}

//-----------------------------------------------------------------------------
// Read binary key file
//-----------------------------------------------------------------------------
void read_key(SBK *s){
    FILE *fp = fopen(SBK_KEY, "rb");
    if( !fp ) perror("Error reading file SBK file."),exit(EXIT_FAILURE);

    unsigned int input[SBK_SIZE];
    size_t size = sizeof(unsigned int);

    fread(input, size, SBK_SIZE, fp);
    if( ferror(fp) ) perror("Error reading SBK file."),exit(EXIT_FAILURE);

    printf("\n");
    printf("Key(from file):\n");
    for(int i = 0; i < SBK_SIZE; i++){
        printf("%d, ", input[i]);
        if(i % 32 == 0 && i != 0) printf("\n");
    }
    printf("\n");

    fclose(fp);
}

//-----------------------------------------------------------------------------
// Output key to screen and binary file
//-----------------------------------------------------------------------------
void write_key(SBK *s){
    FILE *fp = fopen(SBK_KEY, "wb");
    if( !fp ) perror("Error reading file."),exit(1);

    size_t size = sizeof(unsigned int);

    fwrite(s->sbk, size, SBK_SIZE, fp);
    if( ferror(fp) ) perror("Error writing SBK."),exit(EXIT_FAILURE);

    printf("\n");
    printf("Key:\n");
    for(int i = 0; i < SBK_SIZE; i++){
        printf("%d, ", s->sbk[i]);
        if(i % 32 == 0 && i != 0){
            printf("\n");
        }
    }
    printf("\n");

    fclose(fp);
}

//-----------------------------------------------------------------------------
// Compare function for qsort
//-----------------------------------------------------------------------------
int cmp(const void * elem1, const void * elem2){
    indexes * i1, *i2;
    i1 = (indexes*)elem1;
    i2 = (indexes*)elem2;
    return i1->value - i2->value;
}

//-----------------------------------------------------------------------------
// Shuffle the 1 - 128 values using a Knuth shuffle
//-----------------------------------------------------------------------------
void shuffle(SBK *s){

    indexes knuth_sort[SBK_SIZE];

    for(int i = 0; i < SBK_SIZE; i++){
        knuth_sort[i].index = i+1;
        knuth_sort[i].value = s->transposed[i];
    }

    qsort(knuth_sort, SBK_SIZE, sizeof(indexes), cmp);

    printf("Shuffled Output:\n");
    for(int i = 0; i < SBK_SIZE; i++){
        printf("%d: %d\n", knuth_sort[i].index, knuth_sort[i].value);
    }

    for(int i = 0; i < SBK_SIZE; i++){
        s->sbk[i] = knuth_sort[i].index;
    }
}

//-----------------------------------------------------------------------------
// Transpose the 4 arrays of random numbers
//-----------------------------------------------------------------------------
void transpose(SBK *s){
    int counter = 0;
    for(int i = 0; i < 32; i++){
        s->transposed[counter] = s->transpose1[i];
        counter++;
        s->transposed[counter] = s->transpose2[i];
        counter++;
        s->transposed[counter] = s->transpose3[i];
        counter++;
        s->transposed[counter] = s->transpose4[i];
        counter++;
    }

    printf("\nTransposed Numbers:\n");
    for(int i = 0; i < SBK_SIZE; i++){
        printf("%d: %ld\n", i+1, s->transposed[i]);
    }
}

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SBK *s){
    
    pcg64_random_t rng1, rng2, rng3, rng4;

    pcg128_t round = 5;

    pcg128_t s1, s2, s3, s4;

    s1 = *(pcg128_t *)s->seed1;
    s2 = *(pcg128_t *)s->seed2;
    s3 = *(pcg128_t *)s->seed3;
    s4 = *(pcg128_t *)s->seed4;

    pcg64_srandom_r(&rng1, s1, round);
    pcg64_srandom_r(&rng2, s2, round);
    pcg64_srandom_r(&rng3, s3, round);
    pcg64_srandom_r(&rng4, s4, round);

    for(int i = 0; i < TRANSPOSE_SIZE; ++i){
        s->transpose1[i] = abs(pcg64_random_r(&rng1));
        s->transpose2[i] = abs(pcg64_random_r(&rng2));
        s->transpose3[i] = abs(pcg64_random_r(&rng3));
        s->transpose4[i] = abs(pcg64_random_r(&rng4));
    }

    printf("Numbers for array 1 to transpose:\n");
    for(int i = 0; i < TRANSPOSE_SIZE; i++){
        printf("%d: %ld\n", i, s->transpose1[i]);
    }

    printf("Numbers for array 2 to transpose:\n");
    for(int i = 0; i < TRANSPOSE_SIZE; i++){
        printf("%d: %ld\n", i, s->transpose2[i]);
    }

    printf("Numbers for array 3 to transpose:\n");
    for(int i = 0; i < TRANSPOSE_SIZE; i++){
        printf("%d: %ld\n", i, s->transpose3[i]);
    }

    printf("Numbers for array 4 to transpose:\n");
    for(int i = 0; i < TRANSPOSE_SIZE; i++){
        printf("%d: %ld\n", i, s->transpose4[i]);
    }
}

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SBK *s){

    memcpy(s->seed1, s->hash, SEED_SIZE);
    memcpy(s->seed2, s->hash + SEED_SIZE, SEED_SIZE);
    memcpy(s->seed3, s->hash + SEED_SIZE*2, SEED_SIZE);
    memcpy(s->seed4, s->hash + SEED_SIZE*3, SEED_SIZE);

    s->seed1[SBK_SIZE] = '\0';
    s->seed2[SBK_SIZE] = '\0';
    s->seed3[SBK_SIZE] = '\0';
    s->seed4[SBK_SIZE] = '\0';

    printf("\nSeed1 - %d bits:\n", SBK_SIZE);
    for(int i = 0; i < sizeof(s->seed1) - 1 ; i++) {printf("%02x",s->seed1[i]);}
    printf("\n");

    printf("\nSeed2 - %d bits:\n", SBK_SIZE);
    for(int i = 0; i < sizeof(s->seed2) - 1 ; i++) {printf("%02x",s->seed2[i]);}
    printf("\n");

    printf("\nSeed3 - %d bits:\n", SBK_SIZE);
    for(int i = 0; i < sizeof(s->seed3) - 1 ; i++) {printf("%02x",s->seed3[i]);}
    printf("\n");

    printf("\nSeed4 - %d bits:\n", SBK_SIZE);
    for(int i = 0; i < sizeof(s->seed4) - 1 ; i++) {printf("%02x",s->seed4[i]);}
    printf("\n");
}

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SBK *s){
    char* buffer = NULL;
    size_t size = 0;

    FILE *fp = fopen(RSA_KEY, "rb");
    if( !fp ) perror("Error reading private key from RSA."),exit(EXIT_FAILURE);

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    rewind(fp);

    buffer = malloc((size + 1) * sizeof(*buffer));

    fread(buffer, size, 1, fp);
    if( ferror(fp) ) perror("Error reading private key from RSA."),
                        exit(EXIT_FAILURE);

    buffer[size] = '\0';

    printf("%s\n", buffer);

    SHA512((const unsigned char*)buffer, sizeof(buffer) - 1,
            (unsigned char*)s->hash);

    printf("Hash - %ld bits:\n", sizeof(s->hash)*8);
    for(int i = 0; i < sizeof(s->hash) - 1 ; i++) { printf("%02x",s->hash[i]);}
    printf("\n");

    free(buffer);
    fclose(fp);
}

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key(SBK *s){
    int             ret = 0;
    int             bits = 2048;
    unsigned long   e = RSA_F4;
    RSA             *r = NULL;
    BIGNUM          *bne = NULL;
    BIO             *bp_private = NULL;

    bne = BN_new();
    ret = BN_set_word(bne,e);
    if(ret != 1){
        free_r(bp_private, r, bne);
        printf("Failue\n");
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if(ret != 1){
        free_r(bp_private, r, bne);
        printf("Failue\n");
    }

    bp_private = BIO_new_file(RSA_KEY, "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

    free_r(bp_private, r, bne);
}

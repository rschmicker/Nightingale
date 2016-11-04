#include "sub_t.h"

//-----------------------------------------------------------------------------
// Free resources for RSA key creation
//-----------------------------------------------------------------------------
void free_r(BIO *bp_private, RSA *r, BIGNUM *bne){
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);
}

/*
//-----------------------------------------------------------------------------
// Read binary key file
//-----------------------------------------------------------------------------
void read_key(SUB *s){
    FILE *fp = fopen(SUB_KEY, "rb");
    if( !fp ) perror("Error reading file substitution table file."),
                exit(EXIT_FAILURE);

    unsigned char input[SUB_SIZE];
    size_t size = sizeof(unsigned char);

    fread(input, size, SUB_SIZE, fp);
    if( ferror(fp) ) perror("Error reading substitution table file."),
                        exit(EXIT_FAILURE);

    printf("\n");
    printf("Key(from file):\n");
    for(int i = 0; i < SUB_SIZE; i++){
        printf("%d, ", input[i]);
        if(i % 32 == 0 && i != 0) printf("\n");
    }
    printf("\n");

    fclose(fp);
}

//-----------------------------------------------------------------------------
// Output key to screen and binary file
//-----------------------------------------------------------------------------
void write_key(SUB *s){
    FILE *fp = fopen(SUB_KEY, "wb");
    if( !fp ) perror("Error reading file."),exit(1);

    size_t size = sizeof(unsigned char);

    fwrite(s->sub, size, SUB_SIZE, fp);
    if( ferror(fp) ) perror("Error writing substitution table."),
                        exit(EXIT_FAILURE);

    printf("\n");
    printf("Key:\n");
    for(int i = 0; i < SUB_SIZE; i++){
        printf("%d, ", s->sub[i]);
        if(i % 32 == 0 && i != 0){
            printf("\n");
        }
    }
    printf("\n");

    fclose(fp);
}
*/

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
// Shuffle the 1 - 256 values using a Knuth shuffle
//-----------------------------------------------------------------------------
void shuffle(SUB *s){

    indexes knuth_sort[SUB_SIZE];

    for(int i = 0; i < SUB_SIZE; i++){
        knuth_sort[i].index = i+1;
        knuth_sort[i].value = s->sub_rands[i];
    }

    qsort(knuth_sort, SUB_SIZE, sizeof(indexes), cmp);

    /*printf("Shuffled Output:\n");
    for(int i = 0; i < SUB_SIZE; i++){
        printf("%d: %d\n", knuth_sort[i].index, knuth_sort[i].value);
    }*/

    //printf("CHARSSSSS\n");
    for(int i = 0; i < SUB_SIZE; i++){
        s->sub[i] = (unsigned char)knuth_sort[i].index;
        s->reverse_sub[(uint8_t)knuth_sort[i].index] = i;
        //printf("%d -> %c\n", i,  s->sub[i]);
    }
}

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SUB *s){

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

    for(int i = 0; i < 64; ++i){
        s->sub_rands[i] = abs(pcg64_random_r(&rng1));
    }
    for(int i = 64; i < 128; ++i){
        s->sub_rands[i] = abs(pcg64_random_r(&rng2));
    }
    for(int i = 128; i < 192; ++i){
        s->sub_rands[i] = abs(pcg64_random_r(&rng3));
    }
    for(int i = 192; i < 256; ++i){
        s->sub_rands[i] = abs(pcg64_random_r(&rng4));
    }

    /*printf("Substitution Table:\n");
    for(int i = 0; i < SUB_SIZE; i++){
        printf("%d: %lu\n", i, s->sub_rands[i]);
    }*/
}

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SUB *s){

    memcpy(s->seed1, s->hash, SEED_SIZE);
    memcpy(s->seed2, s->hash + SEED_SIZE, SEED_SIZE);
    memcpy(s->seed3, s->hash + SEED_SIZE*2, SEED_SIZE);
    memcpy(s->seed4, s->hash + SEED_SIZE*3, SEED_SIZE);

    s->seed1[SEED_SIZE+1] = '\0';
    s->seed2[SEED_SIZE+1] = '\0';
    s->seed3[SEED_SIZE+1] = '\0';
    s->seed4[SEED_SIZE+1] = '\0';

    /*printf("\nSeed1 - %d bytes:\n", SEED_SIZE);
    for(int i = 0; i < sizeof(s->seed1) - 1 ; i++) {printf("%02x",s->seed1[i]);}
    printf("\n");

    printf("\nSeed2 - %d bytes:\n", SEED_SIZE);
    for(int i = 0; i < sizeof(s->seed2) - 1 ; i++) {printf("%02x",s->seed2[i]);}
    printf("\n");

    printf("\nSeed3 - %d bytes:\n", SEED_SIZE);
    for(int i = 0; i < sizeof(s->seed3) - 1 ; i++) {printf("%02x",s->seed3[i]);}
    printf("\n");

    printf("\nSeed4 - %d bytes:\n", SEED_SIZE);
    for(int i = 0; i < sizeof(s->seed4) - 1 ; i++) {printf("%02x",s->seed4[i]);}
    printf("\n");*/
}

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SUB *s){
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

    //printf("%s\n", buffer);

    SHA512((const unsigned char*)buffer, sizeof(buffer) - 1,
            (unsigned char*)s->hash);

    /*printf("Hash - %ld bits:\n", sizeof(s->hash)*8);
    for(int i = 0; i < sizeof(s->hash) - 1 ; i++) { printf("%02x",s->hash[i]);}
    printf("\n");*/

    free(buffer);
    fclose(fp);
}

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key(SUB *s){
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

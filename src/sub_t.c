#include "sub_t.h"

//-----------------------------------------------------------------------------
void nightgale_dec_set_key(SUB *s){
    generate_hash(s, RSA_KEY);
    generate_seeds(s);
    generate_rands(s);
    shuffle(s);
}

//-----------------------------------------------------------------------------
void nightgale_enc_set_key(SUB *s){
    generate_key();
    generate_hash(s, RSA_KEY);
    generate_seeds(s);
    generate_rands(s);
    shuffle(s);
}

//-----------------------------------------------------------------------------
void nightgale_dec_set_key_v(SUB_V *s){
    generate_hash_v(s, RSA_KEY);
    generate_seeds_v(s);
    generate_rands_v(s);
    shuffle_v(s);
}

//-----------------------------------------------------------------------------
void nightgale_enc_set_key_v(SUB_V *s){
    generate_key();
    generate_hash_v(s, RSA_KEY);
    generate_seeds_v(s);
    generate_rands_v(s);
    shuffle_v(s);
}

//-----------------------------------------------------------------------------
size_t get_file_length(FILE *fp) {
    fseek(fp, 0L, SEEK_CUR);
    long unsigned int mypos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long unsigned int filesize = ftell(fp);
    fseek(fp, mypos, SEEK_SET);
    return filesize;
 }

//-----------------------------------------------------------------------------
// Free resources for RSA key creation
//-----------------------------------------------------------------------------
void free_r(BIO *bp_private, RSA *r, BIGNUM *bne){
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);
}

//-----------------------------------------------------------------------------
// Compare function for qsort
//-----------------------------------------------------------------------------
int cmp(const void *elem1, const void *elem2){
    indexes *i1, *i2;
    i1 = (indexes *)elem1;
    i2 = (indexes *)elem2;
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

    for(int i = 0; i < SUB_SIZE; i++){
        s->sub[i] = (unsigned char)knuth_sort[i].index;
        s->reverse_sub[(unsigned char)knuth_sort[i].index] = i;
    }
}

void shuffle_v(SUB_V *s){
    indexes knuth_sort_low_1[VECTOR_SUB_SIZE];
    indexes knuth_sort_low_2[VECTOR_SUB_SIZE];
    indexes knuth_sort_high_1[VECTOR_SUB_SIZE];
    indexes knuth_sort_high_2[VECTOR_SUB_SIZE];

    for(int i = 0; i < VECTOR_RAND_SIZE; i++){
        knuth_sort_low_1[i].index = i;
        knuth_sort_low_1[i].value = s->sub_rands_1[i];

        knuth_sort_low_2[i].index = i;
        knuth_sort_low_2[i].value = s->sub_rands_2[i];

        knuth_sort_high_1[i].index = i;
        knuth_sort_high_1[i].value = s->sub_rands_3[i];

        knuth_sort_high_2[i].index = i;
        knuth_sort_high_2[i].value = s->sub_rands_4[i];
    }

    qsort(knuth_sort_low_1, VECTOR_RAND_SIZE, sizeof(indexes), cmp);
    qsort(knuth_sort_low_2, VECTOR_RAND_SIZE, sizeof(indexes), cmp);
    qsort(knuth_sort_high_1, VECTOR_RAND_SIZE, sizeof(indexes), cmp);
    qsort(knuth_sort_high_2, VECTOR_RAND_SIZE, sizeof(indexes), cmp);

    unsigned char r_low_temp_1[16];
    unsigned char r_low_temp_2[16];
    unsigned char r_high_temp_1[16];
    unsigned char r_high_temp_2[16];

    unsigned char low_temp_1[16];
    unsigned char low_temp_2[16];
    unsigned char high_temp_1[16];
    unsigned char high_temp_2[16];

    for(int i = 0; i < 16; ++i){
        r_low_temp_1[(unsigned char)knuth_sort_low_1[i].index] = i;
        r_low_temp_2[(unsigned char)knuth_sort_low_2[i].index] = i;
        r_high_temp_1[(unsigned char)knuth_sort_high_1[i].index] = i;
        r_high_temp_2[(unsigned char)knuth_sort_high_2[i].index] = i;

        low_temp_1[i] = (unsigned char)knuth_sort_low_1[i].index;
        low_temp_2[i] = (unsigned char)knuth_sort_low_2[i].index;
        high_temp_1[i] = (unsigned char)knuth_sort_high_1[i].index;
        high_temp_2[i] = (unsigned char)knuth_sort_high_2[i].index;
    }

    for(int i = 0; i < 16; ++i){
        s->r_sub_v_low[i] = r_low_temp_1[i];
        s->r_sub_v_high[i] = r_high_temp_1[i];

        s->sub_v_low[i] = low_temp_1[i];
        s->sub_v_high[i] = high_temp_1[i];
    }

    for(int i = 16; i < 32; ++i){
        s->r_sub_v_low[i] = r_low_temp_2[i - 16];
        s->r_sub_v_high[i] = r_high_temp_2[i - 16];

        s->sub_v_low[i] = low_temp_2[i - 16];
        s->sub_v_high[i] = high_temp_2[i - 16];
    }
}

//-----------------------------------------------------------------------------
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SUB *s){
    pcg64_random_t rng1, rng2, rng3, rng4;
    pcg128_t round = 5;
    pcg128_t s1, s2, s3, s4;

    void *temp;

    temp = s->seed1;
    s1 = *(pcg128_t *)temp;

    temp = s->seed2;
    s2 = *(pcg128_t *)temp;

    temp = s->seed3;
    s3 = *(pcg128_t *)temp;

    temp = s->seed4;
    s4 = *(pcg128_t *)temp;

    pcg64_srandom_r(&rng1, s1, round);
    pcg64_srandom_r(&rng2, s2, round);
    pcg64_srandom_r(&rng3, s3, round);
    pcg64_srandom_r(&rng4, s4, round);

    for(int i = 0; i < 64; ++i){
        s->sub_rands[i] = pcg64_random_r(&rng1);
    }
    for(int i = 64; i < 128; ++i){
        s->sub_rands[i] = pcg64_random_r(&rng2);
    }
    for(int i = 128; i < 192; ++i){
        s->sub_rands[i] = pcg64_random_r(&rng3);
    }
    for(int i = 192; i < 256; ++i){
        s->sub_rands[i] = pcg64_random_r(&rng4);
    }
}

void generate_rands_v(SUB_V *s){
    pcg64_random_t rng1, rng2, rng3, rng4;
    pcg128_t round = 5;
    pcg128_t s1, s2, s3, s4;

    void *temp;

    temp = s->seed1;
    s1 = *(pcg128_t *)temp;

    temp = s->seed2;
    s2 = *(pcg128_t *)temp;

    temp = s->seed3;
    s3 = *(pcg128_t *)temp;

    temp = s->seed4;
    s4 = *(pcg128_t *)temp;

    pcg64_srandom_r(&rng1, s1, round);
    pcg64_srandom_r(&rng2, s2, round);
    pcg64_srandom_r(&rng3, s3, round);
    pcg64_srandom_r(&rng4, s4, round);

    for(int i = 0; i < VECTOR_RAND_SIZE; ++i){
        s->sub_rands_1[i] = pcg64_random_r(&rng1);
    }

    for(int i = 0; i < VECTOR_RAND_SIZE; ++i){
        s->sub_rands_2[i] = pcg64_random_r(&rng2);
    }

    for(int i = 0; i < VECTOR_RAND_SIZE; ++i){
        s->sub_rands_3[i] = pcg64_random_r(&rng3);
    }

    for(int i = 0; i < VECTOR_RAND_SIZE; ++i){
        s->sub_rands_4[i] = pcg64_random_r(&rng4);
    }
}

//-----------------------------------------------------------------------------
// Set up all four seeds from 512 bit hash
//-----------------------------------------------------------------------------
void generate_seeds(SUB *s){
    memcpy(s->seed1, s->hash, SEED_SIZE);
    memcpy(s->seed2, s->hash + SEED_SIZE, SEED_SIZE);
    memcpy(s->seed3, s->hash + SEED_SIZE*2, SEED_SIZE);
    memcpy(s->seed4, s->hash + SEED_SIZE*3, SEED_SIZE);
}

void generate_seeds_v(SUB_V *s){
    memcpy(s->seed1, s->hash, SEED_SIZE);
    memcpy(s->seed2, s->hash + SEED_SIZE, SEED_SIZE);
    memcpy(s->seed3, s->hash + SEED_SIZE*2, SEED_SIZE);
    memcpy(s->seed4, s->hash + SEED_SIZE*3, SEED_SIZE);
}

//-----------------------------------------------------------------------------
// Generate Hash from private key file from RSA
//-----------------------------------------------------------------------------
void generate_hash(SUB *s, const char* key_file){
    FILE *fp = fopen(key_file, "rb");
    if( !fp ) perror("Error reading private key from RSA."),exit(EXIT_FAILURE);

    size_t filesize = get_file_length(fp);
    unsigned char *buffer = (unsigned char*)malloc(filesize);
    size_t nread = fread(buffer, sizeof(unsigned char), filesize, fp);

    if( ferror(fp) || nread != filesize) perror("Error reading private key from RSA."),
                        exit(EXIT_FAILURE);

    SHA512(buffer, filesize, s->hash);
    SHA256(s->hash, SHA512_DIGEST_LENGTH, s->digest);

    free(buffer);
    fclose(fp);
}

void generate_hash_v(SUB_V *s, const char* key_file){
    FILE *fp = fopen(key_file, "rb");
    if( !fp ) perror("Error reading private key from RSA."),exit(EXIT_FAILURE);

    size_t filesize = get_file_length(fp);
    unsigned char *buffer = (unsigned char*)malloc(filesize);
    size_t nread = fread(buffer, sizeof(unsigned char), filesize, fp);

    if( ferror(fp) || nread != filesize) perror("Error reading private key from RSA."),
                        exit(EXIT_FAILURE);

    SHA512(buffer, filesize, s->hash);
    SHA256(s->hash, SHA512_DIGEST_LENGTH, s->digest);

    free(buffer);
    fclose(fp);
}

//-----------------------------------------------------------------------------
// Generate 2048 bit RSA key
//-----------------------------------------------------------------------------
void generate_key(){
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

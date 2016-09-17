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
    FILE *fp = fopen(s->outputname, "rb");
    if( !fp ) perror("Error reading file."),exit(1);

    unsigned short input[SBK_SIZE];
    size_t size = sizeof(short);

    fread(input, size, SBK_SIZE, fp);

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
    FILE *fp = fopen(s->outputname, "wb");
    if( !fp ) perror("Error reading file."),exit(1);

    size_t size = sizeof(short);

    fwrite(s->sbk, size, SBK_SIZE, fp);

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
// Create random ints for shared block key
//-----------------------------------------------------------------------------
void generate_rands(SBK *s){
    unsigned char *buf_seed1 = s->seed1;
    unsigned char *buf_seed2 = s->seed2;
    unsigned char *buf_seed3 = s->seed3;
    unsigned char *buf_seed4 = s->seed4;

    RAND_seed(s->seed1, SEED_SIZE);
    RAND_bytes(buf_seed1, sizeof(buf_seed1)*2);
    RAND_seed(s->seed2, SEED_SIZE);
    RAND_bytes(buf_seed2, sizeof(buf_seed2)*2);
    RAND_seed(s->seed3, SEED_SIZE);
    RAND_bytes(buf_seed3, sizeof(buf_seed3)*2);
    RAND_seed(s->seed4, SEED_SIZE);
    RAND_bytes(buf_seed4, sizeof(buf_seed4)*2);

    printf("\nRandom Number 1:\t");
    for (size_t i = 0; i < sizeof(buf_seed1)*2; i++)
        printf("%02x", buf_seed1[i]);
    printf("\n");
    printf("\nRandom Number 2:\t");
    for (size_t i = 0; i < sizeof(buf_seed2)*2; i++)
        printf("%02x", buf_seed2[i]);
    printf("\n");
    printf("\nRandom Number 3:\t");
    for (size_t i = 0; i < sizeof(buf_seed3)*2; i++)
        printf("%02x", buf_seed3[i]);
    printf("\n");
    printf("\nRandom Number 4:\t");
    for (size_t i = 0; i < sizeof(buf_seed4)*2; i++)
        printf("%02x", buf_seed4[i]);
    printf("\n\n");


    printf("Numbers from seed1: ");
    for(int i = 0; i < SEED_SIZE*2; i++){
        s->sbk[i] = (unsigned short)buf_seed1[i] % SBK_SIZE;
        printf("%d,", s->sbk[i]);
    }
    printf("\n");

    printf("Numbers from seed2: ");
    for(int i = 0; i < SEED_SIZE*2; i++){
        s->sbk[i+32] = (unsigned short)buf_seed2[i] % SBK_SIZE;
        printf("%d,", s->sbk[i+32]);
    }
    printf("\n");

    printf("Numbers from seed3: ");
    for(int i = 0; i < SEED_SIZE*2; i++){
        s->sbk[i+64] = (unsigned short)buf_seed3[i] % SBK_SIZE;
        printf("%d,", s->sbk[i+64]);
    }
    printf("\n");

    printf("Numbers from seed4: ");
    for(int i = 0; i < SEED_SIZE*2; i++){
        s->sbk[i+96] = (unsigned short)buf_seed4[i] % SBK_SIZE;
        printf("%d,", s->sbk[i+96]);
    }
    printf("\n");
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

    FILE *fp = fopen(s->filename, "rb");
    if( !fp ) perror("Error reading file."),exit(1);

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    rewind(fp);

    buffer = malloc((size + 1) * sizeof(*buffer));

    fread(buffer, size, 1, fp);

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

    bp_private = BIO_new_file(s->filename, "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

    free_r(bp_private, r, bne);
}

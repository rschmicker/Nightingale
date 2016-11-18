#include "nightgale_c.h"

//-----------------------------------------------------------------------------
size_t get_file_length(FILE *fp) {
    fseek(fp, 0L, SEEK_CUR);
    long unsigned int mypos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long unsigned int filesize = ftell(fp);
    fseek(fp, mypos, SEEK_SET);
    return filesize;
 }

 void encrypt(NIGHT *n, SUB *s, unsigned char* message, uint64_t *enc_message){

    pcg64u_random_t rng_unique;

    pcg128_t s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique);

    n->anchor = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    n->hamming_mask = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));

    printf("Anchor:  %lu\n", n->anchor);
    printf("hamming: %lu\n", n->hamming_mask);

    unsigned char *word = malloc(WORD_SIZE);
    int round = 0;

    uint64_t previous_word;
    uint64_t current_word;
    uint64_t to_sub;
    uint64_t *b;
    unsigned char *pre_sub;

    // First buffer
    memcpy(message, word, WORD_SIZE);
    message += WORD_SIZE;
    to_sub = n->anchor ^ *(uint64_t *)word ^ n->hamming_mask;
    b = &to_sub;
    pre_sub = (unsigned char*)b;
    for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
    enc_message[round] = to_sub ^ abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    ++round;

    // Every buffer after
    for(int i = 1; i < n->word_count; i+=WORD_SIZE){
        memcpy(message, word, WORD_SIZE);
        message += WORD_SIZE;
        to_sub = enc_message[round-1] ^ *(uint64_t *)word ^ n->hamming_mask;
        b = &to_sub;
        pre_sub = (unsigned char*)b;
        for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
        enc_message[round] = to_sub ^ abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
        ++round;
    }
 }

//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, SUB *s, const char* file){

    // File I/O
    FILE *f_to_enc, *enc, *nkey;
    f_to_enc = fopen(file, "r");
    enc      = fopen(E_FILE, "wb");
    nkey     = fopen(NIGHT_KEY, "wb");

    if( !f_to_enc ) perror("Error reading input file."),exit(EXIT_FAILURE);

    if( !enc ) perror("Error opening encrypted file."),exit(EXIT_FAILURE);

    if( !nkey ) perror("Error opening key file."),exit(EXIT_FAILURE);

    // Length of text to encrypt
    size_t filesize = get_file_length(f_to_enc);
    printf("File length is: %d\n", (int)filesize);

    // Read the whole file into the message buffer
    char *message = malloc(filesize);
    size_t nread = fread(message, sizeof(char), filesize, f_to_enc);
    if (nread != filesize) perror("Error: reading input file...\n"), exit(1);
    

    int to_pad = WORD_SIZE - (filesize % WORD_SIZE);
    n->word_count = filesize / WORD_SIZE;
    n->file_char_length = filesize;

    n->pad = 0;
    if(to_pad != WORD_SIZE){
        n->word_count += 1;
        n->pad = to_pad;
    }  

    uint64_t *enc_message = malloc(sizeof(uint64_t)*filesize);

    //+++++++++++++++++
    // Encrypt here
    //+++++++++++++++++

    double t1, t2;
    t1 = mysecond();
    encrypt(n, s, message, enc_message);
    t2 = mysecond();
    printf("Encrypt Time:\t%fs\n", t2 - t1);

    

    fwrite(n, sizeof(NIGHT), 1, nkey);
    fwrite(enc_message, sizeof(uint64_t), n->word_count, enc);

    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);


    free(message);
    free(enc_message);

    fclose(f_to_enc);
    fclose(enc);
    fclose(nkey);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}


void decrypt(NIGHT *n, SUB *s, unsigned char *decrypt_message, uint64_t *enc_message){

    uint64_t *dec_message = malloc(sizeof(uint64_t)*n->file_char_length);

    int word_count = n->file_char_length / WORD_SIZE;

    pcg64u_random_t rng_unique;

    pcg128_t s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique);

    uint64_t anchor = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    uint64_t hamming_mask = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));

    printf("Anchor:  %lu\n", anchor);
    printf("hamming: %lu\n", hamming_mask);

    unsigned char *word = malloc(WORD_SIZE);
    int round = 0;

    uint64_t previous_word;
    uint64_t current_word;
    uint64_t to_sub;
    uint64_t *b;
    unsigned char *pre_sub;

    //First buffer
    memcpy(enc_message, word, WORD_SIZE);
    enc_message += WORD_SIZE;
    to_sub = *(uint64_t *)word ^ abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    b = &to_sub;
    pre_sub = (unsigned char*)b;
    for(int j = 0; j < WORD_SIZE; ++j) pre_sub[j] = s->reverse_sub[(int)pre_sub[j]];
    dec_message[round] = anchor ^ to_sub ^ hamming_mask;
    ++round;
    
    // Every buffer after
    for(int i = 0; i < word_count; i += WORD_SIZE){
        memcpy(enc_message, word, WORD_SIZE);
        enc_message += WORD_SIZE;
        to_sub = *(uint64_t *)word ^ abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
        b = &to_sub;
        pre_sub = (unsigned char*)b;
        for(int j = 0; j < WORD_SIZE; ++j) pre_sub[j] = s->reverse_sub[(int)pre_sub[j]];
        dec_message[round] = dec_message[round-1] ^ to_sub ^ hamming_mask;
        ++round;
    }

    uint64_t *temp = &dec_message;
    decrypt_message = (unsigned char *)temp;
  
}

//-----------------------------------------------------------------------------
void decrypt_file(const char* cipher_text, const char* night_key_file, const char* rsa_key_file){
    FILE *dcpt, *enc, *nkey;
    enc = fopen(cipher_text, "rb");
    dcpt = fopen(D_FILE, "w");
    nkey = fopen(night_key_file, "rb");

    if( !enc ) perror("Error reading encrypted file."),exit(EXIT_FAILURE);
    if( !dcpt ) perror("Error opening decrypted file."),exit(EXIT_FAILURE);
    if( !nkey ) perror("Error opening key file."),exit(EXIT_FAILURE);

    NIGHT nS, *n;

    int nread = fread(&nS, sizeof(NIGHT), 1, nkey);
    n = &nS;

    if( ferror(nkey) || nread != 1) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    int word_count = n->word_count;
    if(word_count == 0) word_count += 1;

    uint64_t *enc_message = malloc(sizeof(uint64_t)*word_count);

    int nreadenc = fread(enc_message, sizeof(uint64_t), n->word_count, enc);
    
    if( ferror(dcpt) || ferror(enc) || nreadenc != word_count ) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);
    int message_length = n->file_char_length;
    if(message_length < WORD_SIZE) message_length = n->file_char_length;
    printf("Message Length: %d\n", message_length);

    SUB s;
    printf("Gen hash...\n");
    generate_hash(&s, rsa_key_file);
    printf("Gen seeds...\n");
    generate_seeds(&s);
    printf("Gen rands...\n");
    generate_rands(&s);
    printf("Shuffle...\n");
    shuffle(&s);

    //++++++++++++++++++
    // Decrypt
    //++++++++++++++++++
    unsigned char *decrypt_message = malloc(message_length);

    double t1, t2;
    t1 = mysecond();
    decrypt(n, &s, decrypt_message, enc_message);
    t2 = mysecond();
    printf("Decrypt Time: %fs\n", t2 - t1);

    fwrite(decrypt_message, sizeof(char), n->file_char_length, dcpt);

    free(decrypt_message);
    free(enc_message);

    fclose(dcpt);
    fclose(enc);
    fclose(nkey);
}

#include "nightgale_c.h"

//-----------------------------------------------------------------------------
unsigned char *encrypt(NIGHT *n, SUB *s, const unsigned char* message){

    uint64_t *enc_message = malloc(sizeof(uint64_t)*n->word_count);
    uint64_t *plain_text = (uint64_t *)message;

    // PNRG initialization
    pcg64_random_t rng_unique, rng_anch, rng_ham;
    pcg128_t s1_unique = *(pcg128_t *)s->seed1;
    pcg128_t anchor_seed = *(pcg128_t *)&s->digest[0];
    pcg128_t ham_seed = *(pcg128_t *)&s->digest[SHA256_DIGEST_LENGTH/2];
    pcg64_srandom_r(&rng_unique, s1_unique, 5);
    pcg64_srandom_r(&rng_anch, anchor_seed, 6);
    pcg64_srandom_r(&rng_ham, ham_seed, 7);

    // Anchor must call the PNRG first
    uint64_t anchor = pcg64_random_r(&rng_anch), root;
    printf("test: %lu\n", anchor);
    uint64_t hamming_mask = pcg64_random_r(&rng_ham);
    
    unsigned char *word = malloc(WORD_SIZE), *pre_sub;
    uint64_t decimal_word;

    // Encrypt buffers
    int round = 0;
    root = anchor;
    for(int i = 0; i < n->word_count; ++i){
        decimal_word = root ^ plain_text[i] ^ hamming_mask;
        pre_sub = (unsigned char *)&decimal_word;
        for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
        enc_message[i] = decimal_word ^ pcg64_random_r(&rng_unique);
        root = decimal_word;
    }
    return (unsigned char *)enc_message;
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
    n->word_count = filesize / WORD_SIZE;
    if(filesize % WORD_SIZE != 0) ++n->word_count;
    n->file_length = filesize;

    // Read the whole file into the message buffer including any padding
    n->pad = WORD_SIZE - filesize % WORD_SIZE;
    if(n->pad == 8) n->pad = 0;
    printf("pad: %d\n", n->pad);
    unsigned char *message = calloc(sizeof(unsigned char), filesize + n->pad);

    size_t nread = fread(message, sizeof(unsigned char), filesize, f_to_enc);
    if (nread != filesize) perror("Error: reading input file...\n"), exit(1);

    // Encrypt here
    double t1, elapsed;
    t1 = mysecond();
    unsigned char *enc_message = encrypt(n, s, message);
    t1 = mysecond() - t1;
    double rate = (((double)filesize)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // Write encrypted message to file
    fwrite(n, sizeof(NIGHT), 1, nkey);
    fwrite(enc_message, sizeof(unsigned char), n->file_length + n->pad, enc);
    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);

    // Clean up
    free(message);
    free(enc_message);
    fclose(f_to_enc);
    fclose(enc);
    fclose(nkey);
}

//-----------------------------------------------------------------------------
unsigned char *decrypt(NIGHT *n, SUB *s, 
                        const unsigned char *encrypted_message){ 

    uint64_t *enc_message = (uint64_t *)encrypted_message;   

    uint64_t *dec_message = malloc(sizeof(uint64_t)*n->word_count);

    // PNRG initialization
    pcg64_random_t rng_unique, rng_anch, rng_ham;
    pcg128_t s1_unique = *(pcg128_t *)s->seed1;
    pcg128_t anchor_seed = *(pcg128_t *)&s->digest[0];
    pcg128_t ham_seed = *(pcg128_t *)&s->digest[SHA256_DIGEST_LENGTH/2];
    pcg64_srandom_r(&rng_unique, s1_unique, 5);
    pcg64_srandom_r(&rng_anch, anchor_seed, 6);
    pcg64_srandom_r(&rng_ham, ham_seed, 7);

    // Anchor must call the PNRG first
    uint64_t anchor = pcg64_random_r(&rng_anch), root;
    printf("test: %lu\n", anchor);
    uint64_t hamming_mask = pcg64_random_r(&rng_ham);

    unsigned char *word = malloc(WORD_SIZE), *pre_sub;
    uint64_t decimal_word, pre_sub_decimal_word;

    // Decrypt here
    int round = 0;
    root = anchor;
    for(int i = 0; i < n->word_count; ++i) {
        uint64_t key = pcg64_random_r(&rng_unique);
        decimal_word = enc_message[i] ^ key;
        pre_sub_decimal_word = decimal_word;
        pre_sub = (unsigned char *)&decimal_word;
        for(int j = 0; j < WORD_SIZE; ++j) pre_sub[j] = s->reverse_sub[(int)pre_sub[j]];
        dec_message[i] = root ^ decimal_word ^ hamming_mask;
        root = pre_sub_decimal_word;
    }
    return (unsigned char *)dec_message;
}

//-----------------------------------------------------------------------------
void decrypt_file(const char* cipher_text, const char* night_key_file, 
                    const char* rsa_key_file){

    // File I/O
    FILE *dcpt, *enc, *nkey;
    enc = fopen(cipher_text, "rb");
    dcpt = fopen(D_FILE, "w");
    nkey = fopen(night_key_file, "rb");
    if( !enc ) perror("Error reading encrypted file."),exit(EXIT_FAILURE);
    if( !dcpt ) perror("Error opening decrypted file."),exit(EXIT_FAILURE);
    if( !nkey ) perror("Error opening key file."),exit(EXIT_FAILURE);

    // Read in the Night Key
    NIGHT nS, *n;
    int nread = fread(&nS, sizeof(NIGHT), 1, nkey);
    n = &nS;
    if( ferror(nkey) || nread != 1) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    // Read the encrypted text from file
    unsigned char *encrypted_message = calloc(sizeof(unsigned char), 
                                                n->file_length + n->pad);

    int nreadenc = fread(encrypted_message, sizeof(unsigned char), n->file_length + n->pad, enc);
    if( ferror(dcpt) || ferror(enc) || nreadenc != n->file_length + n->pad) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    printf("File length is: %d\n", n->file_length);

    // Generate substitution table from RSA key
    SUB s;
    printf("Gen hash...\n");
    generate_hash(&s, rsa_key_file);
    printf("Gen seeds...\n");
    generate_seeds(&s);
    printf("Gen rands...\n");
    generate_rands(&s);
    printf("Shuffle...\n");
    shuffle(&s);

    // Decrypt here
    double t1;
    t1 = mysecond();
    unsigned char *decrypt_message = decrypt(n, &s, encrypted_message);
    t1 = mysecond() - t1;
    double rate = (((double)n->file_length)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Decrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // Write decrypted text to file
    fwrite(decrypt_message, sizeof(unsigned char), n->file_length, dcpt);

    // Clean up
    free(decrypt_message);
    free(encrypted_message);
    fclose(dcpt);
    fclose(enc);
    fclose(nkey);
}

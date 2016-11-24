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

//-----------------------------------------------------------------------------
unsigned char *encrypt(NIGHT *n, SUB *s, const unsigned char* message){

    uint64_t *enc_message = malloc(sizeof(uint64_t)*n->word_count);
    uint64_t *plain_text = (uint64_t *)message;

    // PNRG initialization
    pcg64_random_t rng_unique;
    pcg128_t s1_unique = *(pcg128_t *)s->seed1;
    pcg64_srandom_r(&rng_unique, s1_unique, 5);

    // Anchor must call the PNRG first
    uint64_t anchor = pcg64_random_r(&rng_unique), root;
    uint64_t hamming_mask = pcg64_random_r(&rng_unique);

    unsigned char *word = malloc(WORD_SIZE), *pre_sub;
    uint64_t decimalWord;

    // Encrypt buffers
    int round = 0;
    root = anchor;
    for(int i = 0; i < n->word_count; ++i){
        decimalWord = root ^ plain_text[i] ^ hamming_mask;
        pre_sub = (unsigned char *)&decimalWord;
        for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
        enc_message[i] = decimalWord ^ pcg64_random_r(&rng_unique);
        root = enc_message[i];
    }

    // Pad if necessary

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
    n->file_char_length = filesize;

    // Read the whole file into the message buffer
    char *message = malloc(filesize);
    size_t nread = fread(message, sizeof(char), filesize, f_to_enc);
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
    fwrite(enc_message, sizeof(uint64_t), n->word_count, enc);
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
unsigned char *decrypt(NIGHT *n, SUB *s, const unsigned char *e){ 

    uint64_t *enc_message = (uint64_t *)e;   

    int word_count = n->file_char_length / WORD_SIZE;
    uint64_t *dec_message = malloc(sizeof(uint64_t)*word_count);

    // PRNG initilization
    pcg64_random_t rng_unique;
    pcg128_t s1_unique = *(pcg128_t *)s->seed1;
    pcg64_srandom_r(&rng_unique, s1_unique, 5);

    // Anchor needs to call the random number generator first
    uint64_t anchor = pcg64_random_r(&rng_unique), root;
    uint64_t hamming_mask = pcg64_random_r(&rng_unique);

    unsigned char *word = malloc(WORD_SIZE), *pre_sub;
    uint64_t decimalWord;

    // Decrypt here
    int round = 0;
    root = anchor;
    for(int i = 0; i < word_count; ++i) {
        decimalWord = enc_message[i] ^ pcg64_random_r(&rng_unique);
        pre_sub = (unsigned char *)&decimalWord;
        for(int j = 0; j < WORD_SIZE; ++j) pre_sub[j] = s->reverse_sub[(int)pre_sub[j]];
        dec_message[i] = root ^ decimalWord ^ hamming_mask;
        root = enc_message[i];
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

    // Calculate the number of words
    int word_count = n->word_count;
    if(word_count == 0) word_count += 1;

    // Read the encrypted text from file
    //uint64_t *enc_message = malloc(sizeof(uint64_t)*word_count);
    //int nreadenc = fread(enc_message, sizeof(uint64_t), word_count, enc);
    unsigned char *e = malloc(n->file_char_length);
    int nreadenc = fread(e, sizeof(char), n->file_char_length, enc);
    if( ferror(dcpt) || ferror(enc) || nreadenc != n->file_char_length ) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    printf("File length is: %d\n", n->file_char_length);

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
    unsigned char *decrypt_message = decrypt(n, &s, e);
    t1 = mysecond() - t1;
    double rate = (((double)n->file_char_length)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Decrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // Write decrypted text to file
    fwrite(decrypt_message, sizeof(unsigned char), n->file_char_length, dcpt);

    // Clean up
    free(decrypt_message);
    free(e);
    fclose(dcpt);
    fclose(enc);
    fclose(nkey);
}

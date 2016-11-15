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

 void encrypt(NIGHT *n, SUB *s, unsigned char* message, uint64_t *enc_message,
                 uint64_t *keys){
    pcg64u_random_t rng_unique;

    pcg128_t s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique);

    n->anchor = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    n->hamming_mask = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));

    
    uint64_t *words = malloc(sizeof(uint64_t)*n->word_count);

    for(int i = 0; i < n->word_count; ++i){
        keys[i] = abs(pcg_unique_128_xsh_rs_64_random_r(&rng_unique));
    }

    unsigned char *word = malloc(WORD_SIZE);
    int round = 0;

    // Handle if message is less than word size
    if(n->file_char_length < WORD_SIZE){
        // Fill word as much as possible
        for(int p = 0; p < n->file_char_length; ++p) word[p] = s->sub[(int)message[p]];
        // Pad the rest
        for(int k = n->file_char_length; k < WORD_SIZE; ++k) word[k] = (unsigned char)'0';

        uint64_t binary = *(uint64_t *)word;
        words[round] = binary;
        uint64_t to_sub = n->anchor ^ binary ^ n->hamming_mask;
        uint64_t *b = &to_sub;
        unsigned char* pre_sub = (unsigned char*)b;
        for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
        enc_message[round] = to_sub ^ keys[round];
    }
    else{
        for(int i = 0; ; ++i){
            // First case for when i = the Word size, use the anchor
            if(i == WORD_SIZE){
                uint64_t binary = *(uint64_t *)word;
                words[round] = binary;
                uint64_t to_sub = n->anchor ^ binary ^ n->hamming_mask;
                uint64_t *b = &to_sub;
                unsigned char* pre_sub = (unsigned char*)b;
                for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
                enc_message[round] = to_sub ^ keys[round];
                ++round;
                memset(word, 0, sizeof(word));
            }
            // Every word after 8 i.e. 16, 24, 32, etc.
            if(i != 0 && i % WORD_SIZE == 0 && i != WORD_SIZE){
                uint64_t binary = *(uint64_t *)word;
                words[round] = binary;
                uint64_t to_sub = words[round -1] ^ binary ^ n->hamming_mask;
                uint64_t *b = &to_sub;
                unsigned char* pre_sub = (unsigned char*)b;
                for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
                enc_message[round] = to_sub ^ keys[round];
                ++round;
                memset(word, 0, sizeof(word));
            }
            // Pad the remaning characters if message does not evenly divide by 8
            // and handle breaking out of the loop
            if(i == n->file_char_length){
                if(n->pad != 0){
                    for(int j = i%WORD_SIZE; j < WORD_SIZE; ++j) word[j] = (unsigned char)'0';
                    uint64_t binary = *(uint64_t *)word;
                    words[round] = binary;
                    uint64_t to_sub = words[round -1] ^ binary ^ n->hamming_mask;
                    uint64_t *b = &to_sub;
                    unsigned char* pre_sub = (unsigned char*)b;
                    for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
                    enc_message[round] = to_sub ^ keys[round];
                }
                break;                
            }
            word[i%WORD_SIZE] = message[i];
        }
    }
 }

//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, SUB *s, const char* file){

    // File I/O
    FILE *f_to_enc, *enc, *fkey;
    f_to_enc = fopen(file, "r");
    enc      = fopen(E_FILE, "wb");
    fkey     = fopen(KEY, "wb");

    if( !f_to_enc ) perror("Error reading input file."),exit(EXIT_FAILURE);

    if( !enc ) perror("Error opening encrypted file."),exit(EXIT_FAILURE);

    if( !fkey ) perror("Error opening key file."),exit(EXIT_FAILURE);

    // Length of text to encrypt
    size_t filesize = get_file_length(f_to_enc);
    printf("File length is: %d\n", (int)filesize);

    // Read the whole file into the message buffer
    char *message = malloc(filesize);
    size_t nread = fread(message, sizeof(char), filesize, f_to_enc);
    if (nread != filesize) perror("Error: reading input file...\n"), exit(1);
    //uint64_t enc_message[filesize];
    

    int to_pad = WORD_SIZE - (filesize % WORD_SIZE);
    n->word_count = filesize / WORD_SIZE;
    n->file_char_length = filesize;

    n->pad = 0;
    if(to_pad != WORD_SIZE){
        n->word_count += 1;
        n->pad = to_pad;
    }  

    uint64_t *enc_message = malloc(sizeof(uint64_t)*filesize);
    uint64_t *keys = malloc(sizeof(uint64_t)*n->word_count);

    //+++++++++++++++++
    // Encrypt here
    //+++++++++++++++++

    double t1, t2;
    t1 = mysecond();
    encrypt(n, s, message, enc_message, keys);
    t2 = mysecond();
    printf("Encrypt Time: %fs\n", t2 - t1);

    

    fwrite(n, sizeof(NIGHT), 1, fkey);
    fwrite(enc_message, sizeof(uint64_t), n->word_count, enc);
    fwrite(keys, sizeof(uint64_t), n->word_count, fkey);
    fwrite(s, sizeof(SUB), 1, fkey);

    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);


    free(message);
    fclose(f_to_enc);
    fclose(enc);
    fclose(fkey);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}


void decrypt(NIGHT *n, SUB *s, unsigned char *decrypt_message, 
                uint64_t *enc_message, uint64_t *keys, uint64_t *binary_mes){

    for(int round = 0; round < n->word_count; ++round){
        binary_mes[round] = enc_message[round] ^ keys[round];
        uint64_t *b = &binary_mes[round];
        unsigned char *message = (unsigned char*)b;
        for(int j = 0; j < WORD_SIZE; ++j){
            message[j] = s->reverse_sub[(int)message[j]];
        }
        if(round == 0){
            binary_mes[round] = n->anchor ^ binary_mes[round] ^ n->hamming_mask;
            //strncpy(decrypt_message, message, WORD_SIZE);
        }
        else{
            binary_mes[round] = binary_mes[round] ^ binary_mes[round-1] ^ n->hamming_mask;
            //strncat(decrypt_message, message, WORD_SIZE);
        }
        for(int k = round*WORD_SIZE; k < round*WORD_SIZE+WORD_SIZE; ++k){
            decrypt_message[k] = message[k%WORD_SIZE];
        }
    }
}

//-----------------------------------------------------------------------------
void decrypt_file(const char* cipher_text, const char* key_file){
    FILE *dcpt, *enc, *fkey;
    enc = fopen(cipher_text, "rb");
    dcpt = fopen(D_FILE, "w");
    fkey = fopen(key_file, "rb");

    if( !enc ) perror("Error reading encrypted file."),exit(EXIT_FAILURE);
    if( !dcpt ) perror("Error opening decrypted file."),exit(EXIT_FAILURE);
    if( !fkey ) perror("Error opening key file."),exit(EXIT_FAILURE);

    NIGHT nS, *n;
    SUB sS, *s;

    int nread = fread(&nS, sizeof(NIGHT), 1, fkey);
    n = &nS;

    if( ferror(fkey) || nread != 1) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    int word_count = n->word_count;
    if(word_count == 0) word_count += 1;

    uint64_t *enc_message = malloc(sizeof(uint64_t)*word_count);
    uint64_t *keys = malloc(sizeof(uint64_t)*word_count);
    uint64_t *binary_mes = malloc(sizeof(uint64_t)*word_count);

    int nreadenc = fread(enc_message, sizeof(uint64_t), n->word_count, enc);
    int nreadkeys = fread(keys, sizeof(uint64_t), n->word_count, fkey);
    int nreadSUB = fread(&sS, sizeof(SUB), 1, fkey);
    s = &sS;
    
    if( ferror(dcpt) || ferror(enc) || nreadenc != word_count || nreadkeys != word_count || nreadSUB != 1) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);
    int message_length = n->file_char_length;
    if(message_length < WORD_SIZE) message_length = n->file_char_length;
    printf("Message Length: %d\n", message_length);

    //++++++++++++++++++
    // Decrypt
    //++++++++++++++++++
    unsigned char *decrypt_message = malloc(message_length);

    double t1, t2;
    t1 = mysecond();
    decrypt(n, s, decrypt_message, enc_message, keys, binary_mes);
    t2 = mysecond();
    printf("Decrypt Time: %fs\n", t2 - t1);

    fwrite(decrypt_message, sizeof(char), n->file_char_length, dcpt);

    fclose(dcpt);
    fclose(enc);
    fclose(fkey);
}

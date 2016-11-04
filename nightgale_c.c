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
    uint64_t *enc_message = malloc(sizeof(uint64_t)*filesize);

    int to_pad = WORD_SIZE - (filesize % WORD_SIZE);
    int word_count = filesize / WORD_SIZE;
    printf("WORD_SIZE, to_pad, word_count = %d, %d, %d\n", WORD_SIZE, to_pad, word_count);

    n->pad = 0;
    if(to_pad != WORD_SIZE){
        word_count += 1;
        n->pad = to_pad;
    }  

    printf("n->pad, WORD_SIZE, to_pad, word_count = %d, %d, %d, %d\n", n->pad, WORD_SIZE, to_pad, word_count);

    //+++++++++++++++++
    // Encrypt here
    //+++++++++++++++++
    pcg64u_random_t rng_unique;

    pcg128_t s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique);

    n->anchor = abs(pcg64u_random_r(&rng_unique));

    uint64_t *keys = malloc(sizeof(uint64_t)*word_count);
    uint64_t *words = malloc(sizeof(uint64_t)*word_count);
    uint64_t *enc_mes = malloc(sizeof(uint64_t)*word_count);
/*
    uint64_t keys   [word_count];
    uint64_t words  [word_count];
    uint64_t enc_mes[word_count];
*/

    for(int i = 0; i < word_count; ++i){
        keys[i] = abs(pcg64u_random_r(&rng_unique));
/*
        printf("key %d: %lu\n", i, keys[i]);
*/
    }

    // unsigned char word[WORD_SIZE];
    unsigned char *word = malloc(WORD_SIZE);
    int round = 0;

    // Handle if message is less than word size
    if(filesize < WORD_SIZE){
        // Fill word as much as possible
        for(int p = 0; p < filesize; ++p) word[p] = s->sub[(int)message[p]];
        // Pad the rest
        for(int k = filesize; k < WORD_SIZE; ++k) word[k] = (unsigned char)'0';

        uint64_t binary = *(uint64_t *)word;
        words[round] = binary;
/*
        printf("only word: %lu\n", words[round]);
*/
        enc_message[round] = n->anchor ^ binary ^ keys[round];
    }
    else{
        for(int i = 0; ; ++i){
            // First case for when i = the Word size, use the anchor
            if(i == WORD_SIZE){
                uint64_t binary = *(uint64_t *)word;
                words[round] = binary;
/*
                printf("first word: %lu\n", words[round]);
*/
                enc_message[round] = n->anchor ^ binary ^ keys[round];
                ++round;
                memset(word, 0, sizeof(word));
            }
            // Every word after 8 i.e. 16, 24, 32, etc.
            if(i != 0 && i % WORD_SIZE == 0 && i != WORD_SIZE){
                uint64_t binary = *(uint64_t *)word;
                words[round] = binary;
                enc_message[round] = words[round - 1] ^ binary ^ keys[round];
/*
                printf("previous word: %lu\tnew word: %lu\n", words[round - 1], binary);
*/
                ++round;
                memset(word, 0, sizeof(word));
            }
            // Pad the remaning characters if message does not evenly divide by 8
            // and handle breaking out of the loop
            if(i == filesize){
                if(to_pad != 0){
                    for(int j = i%WORD_SIZE; j < WORD_SIZE; ++j) word[j] = (unsigned char)'0';
                    uint64_t binary = *(uint64_t *)word;
                    words[round] = binary;
                    enc_message[round] = words[round - 1] ^ binary ^ keys[round];
/*
                    printf("previous word: %lu\tnew word: %lu\n", words[round - 1], binary);
*/
                }
                break;                
            }
            word[i%WORD_SIZE] = s->sub[(int)message[i]];
/*
            printf("%d: round: %d enc: %c\t mes: %c\n", i, round, word[i%WORD_SIZE], message[i]);
*/
        }
    }
    

    n->file_char_length = filesize;

/*
    for(int i = 0; i < word_count; ++i){
        printf("enc word %d: %lu\n", i, enc_message[i]);
    }
*/

    n->word_count = word_count;

/*
    printf("Encrypted Message\n%s\n", (unsigned char*)enc_message);
*/

    fwrite(n, sizeof(NIGHT), 1, fkey);
    fwrite(enc_message, sizeof(uint64_t), word_count + 1, enc);
    fwrite(keys, sizeof(uint64_t), word_count + 1, fkey);
    fwrite(s, sizeof(SUB), 1, fkey);

    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);

    free(message);
    fclose(f_to_enc);
    fclose(enc);
    fclose(fkey);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
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
    SUB s;

    fread(&nS, sizeof(NIGHT), 1, fkey);
    n = &nS;
    

    if( ferror(fkey) ) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

    int word_count = n->word_count;
    if(word_count == 0) word_count += 1;

    uint64_t *enc_message = malloc(sizeof(uint64_t)*word_count);
    uint64_t *keys = malloc(sizeof(uint64_t)*word_count);
    uint64_t *binary_mes = malloc(sizeof(uint64_t)*word_count);
/*
    uint64_t enc_message[word_count];
    uint64_t keys[word_count];
    uint64_t binary_mes[word_count];
*/

    fread(enc_message, sizeof(uint64_t), word_count + 1, enc);
    fread(keys, sizeof(uint64_t), word_count + 1, fkey);

    fread(&s, sizeof(SUB), 1, fkey);
    
    if( ferror(dcpt) || ferror(enc) ) 
                        perror("Error reading encrypt/decrypt/key file."),
                        exit(EXIT_FAILURE);

/*
    for(int i = 0; i < word_count; ++i){
        printf("key %d: %lu\n", i, keys[i]);
    }

    for(int i = 0; i < word_count; ++i){
        printf("enc word %d: %lu\n", i, enc_message[i]);
    }
*/

    int message_length = n->file_char_length;
    //if(n->pad != 0) message_length = n->file_char_length - n->pad;
    if(message_length < WORD_SIZE) message_length = n->file_char_length;
    printf("Message Length: %d\n", message_length);

    //++++++++++++++++++
    // Decrypt
    //++++++++++++++++++
    binary_mes[0] = n->anchor ^ enc_message[0] ^ keys[0]; 
/*
    printf("original word %d: %lu\n", 0, binary_mes[0]);
*/

    for(int i = 1; i < word_count; ++i){
/*
        printf("b: %lu ^ enc: %lu ^ key %lu\n", binary_mes[i-1] , enc_message[i] , keys[i]);
*/
        binary_mes[i] = binary_mes[i-1] ^ enc_message[i] ^ keys[i];
/*
        printf("original word %d: %lu\n", i, binary_mes[i]);
*/
    }

    uint64_t *b = &binary_mes[0];
    unsigned char *message = (unsigned char*)b;
    //message[n->file_char_length] = '\0';

/*
    printf("Message before sub table:\n");
    printf("%s\n", message);

    printf("Original Message:\n");
*/
    
    

    //unsigned char decrypt_message[message_length];
    unsigned char *decrypt_message = malloc(message_length);

    for(int i = 0; i < message_length; ++i){
        decrypt_message[i] = s.reverse_sub[(int)message[i]];
/*
        printf("%c", decrypt_message[i]);
*/
        fprintf(dcpt, "%c", decrypt_message[i]);
    }

/*
    printf("\n");
*/

    fclose(dcpt);
    fclose(enc);
    fclose(fkey);
}

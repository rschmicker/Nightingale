#include "nightgale_c.h"

//-----------------------------------------------------------------------------
long long int get_file_length(FILE *fp) {
    fseek(fp, 0L, SEEK_CUR);
    long long int mypos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long long int filesize = ftell(fp);
    fseek(fp, mypos, SEEK_SET);
    return filesize;
 }

//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, SUB *s, const char* file){
    FILE *f_to_enc, *enc;
    f_to_enc = fopen(file, "r");
    enc = fopen(E_FILE, "wb");

    if( !f_to_enc ) perror("Error reading input file."),exit(EXIT_FAILURE);

    if( !enc ) perror("Error opening encrypted file."),exit(EXIT_FAILURE);

    long long int filesize = get_file_length(f_to_enc);
    StringInfo message_info = makeStringInfo();
    enlargeStringInfo(message_info, filesize);

    char *message = message_info->data;
    

    // Read the whole file into the message buffer
    long long int nread = fread(message, 1, filesize, f_to_enc);
    if (nread != filesize) perror("Error: reading input file...\n"), exit(1);

    message[filesize] = '\0';
    

    int m_size = filesize;

    // TODO: this shouldn't be necessary - we may be encoding raw data, so we aren't using string conventions
    // message[m_size] = (unsigned char)'\0';

    uint64_t enc_message[m_size];

    printf("\nOriginal Message:\n");
    printf("%s\n", message);

    //+++++++++++++++++
    // Encrypt here
    //+++++++++++++++++
    pcg64u_random_t rng_unique;

    pcg128_t s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique);

    n->anchor = abs(pcg64u_random_r(&rng_unique));

    uint64_t keys[m_size / WORD_SIZE];
    uint64_t words[m_size / WORD_SIZE];
    uint64_t enc_mes[m_size / WORD_SIZE];

    for(int i = 0; i < m_size / WORD_SIZE; ++i){
        keys[i] = abs(pcg64u_random_r(&rng_unique));
        printf("key %d: %lu\n", i, keys[i]);
    }

    unsigned char word[WORD_SIZE];
    int round = 0;
    for(int i = 0; i < m_size + 1; ++i){
        if(i == WORD_SIZE){
            uint64_t binary = *(uint64_t *)word;
            words[round] = binary;
            printf("first word: %lu\n", words[round]);
            enc_message[round] = n->anchor ^ binary ^ keys[round];
            ++round;
            memset(word, 0, sizeof(word));
        }
        if(i != 0 && i % WORD_SIZE == 0 && i != WORD_SIZE){
            uint64_t binary = *(uint64_t *)word;
            words[round] = binary;
            enc_message[round] = words[round - 1] ^ binary ^ keys[round];
            printf("previous word: %lu\tnew word: %lu\n", words[round - 1], words[round - 1] ^ binary ^ keys[round]);
            ++round;
            memset(word, 0, sizeof(word));
            if(i + 1 == m_size) break;
        }
        word[i%WORD_SIZE] = s->sub[(int)message[i]];
        printf("%d: round: %d enc: %c\t mes: %c\n", i, round, word[i%WORD_SIZE], message[i]);
    }

    for(int i = 0; i < n->file_char_length / WORD_SIZE + 1; ++i){
        printf("enc word %d: %lu\n", i, enc_message[i]);
    }

    n->file_char_length = m_size;

    printf("Encrypted Message\n%s\n", (unsigned char*)enc_message);

    fwrite(n, sizeof(NIGHT), 1, enc);
    fwrite(enc_message, sizeof(uint64_t), m_size / WORD_SIZE + 1, enc);
    fwrite(keys, sizeof(uint64_t), m_size / WORD_SIZE + 1, enc);

    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);

    free(message);
    fclose(f_to_enc);
    fclose(enc);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

//-----------------------------------------------------------------------------
void decrypt_file(NIGHT *n, SUB *s){
    FILE *dcpt, *enc;
    enc = fopen(E_FILE, "rb");
    dcpt = fopen(D_FILE, "w");

    if( !enc ) perror("Error reading encrypted file."),exit(EXIT_FAILURE);
    if( !dcpt ) perror("Error opening decrypted file."),exit(EXIT_FAILURE);

    unsigned char *message;
    uint64_t enc_message[n->file_char_length / WORD_SIZE];
    uint64_t keys[n->file_char_length / WORD_SIZE];
    uint64_t binary_mes[n->file_char_length / WORD_SIZE];

    fread(n, sizeof(NIGHT), 1, enc);
    fread(enc_message, sizeof(uint64_t), n->file_char_length / WORD_SIZE + 1, enc);
    fread(keys, sizeof(uint64_t), n->file_char_length / WORD_SIZE + 1, enc);

    if( ferror(enc) || ferror(dcpt) ) 
                        perror("Error reading encrypt/decrypt file."),
                        exit(EXIT_FAILURE);

    for(int i = 0; i < n->file_char_length / WORD_SIZE; ++i){
        printf("key %d: %lu\n", i, keys[i]);
    }

    for(int i = 0; i < n->file_char_length / WORD_SIZE; ++i){
        printf("enc word %d: %lu\n", i, enc_message[i]);
    }

    //++++++++++++++++++
    // Decrypt
    //++++++++++++++++++
    binary_mes[0] = n->anchor ^ enc_message[0] ^ keys[0]; 
    printf("original word %d: %lu\n", 0, binary_mes[0]);

    for(int i = 1; i < n->file_char_length / WORD_SIZE; ++i){
        printf("b: %lu ^ enc: %lu ^ key %lu\n", binary_mes[i-1] , enc_message[i] , keys[i]);
        binary_mes[i] = binary_mes[i-1] ^ enc_message[i] ^ keys[i];
        printf("original word %d: %lu\n", i, binary_mes[i]);
    }

    message = (unsigned char *)binary_mes;

    printf("Original Message:\n%s\n", message);
    for(int i = 0; i < n->file_char_length; ++i){
        fprintf(dcpt, "%c", message[i]);
    }

    fclose(dcpt);
    fclose(enc);
}
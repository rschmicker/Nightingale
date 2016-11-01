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
    

    int m_size = filesize;

    // TODO: this shouldn't be necessary - we may be encoding raw data, so we aren't using string conventions
    // message[m_size] = (unsigned char)'\0';

    unsigned char enc_message[m_size];

    printf("\nOriginal Message:\n");
    printf("%s\n", message);

    //+++++++++++++++++
    // Encrypt here
    //+++++++++++++++++
    pcg64u_random_t rng_unique;

    s1_unique = *(pcg128_t *)s->seed1;

    pcg64u_srandom_r(&rng_unique, s1_unique, 5);

    n->anchor = abs(pcg64u_random_r(&rng_unique));

    uint64_t keys[m_size / WORD_SIZE];
    uint64_t words[m_size / WORD_SIZE];
    uint64_t enc_mes[m_size / WORD_SIZE]

    for(int i = 0; i < m_size / WORD_SIZE; ++i){
        keys[i] = abs(pcg64u_random_r(&rng_unique));
    }

    unsigned char word[WORD_SIZE];
    int round = 0;
    for(int i = 0; i < m_size; ++i){
        if(i == WORD_SIZE){
            uint64_t binary = *(uint64_t *)word;
            words[round] = binary;
            uint64_t result = anchor ^ binary;
            uint64_t chunk = result ^ keys[round];
            enc_message[round] = chunk;
            ++round;
            memset(word, 0, sizeof(word));
        }
        if(i != 0 && m_size % WORD_SIZE == 0){
            uint64_t binary = *(uint64_t *)word;
            words[round] = binary;
            uint64_t result = words[round - 1] ^ binary;
            uint64_t chunk = result ^ keys[round];
            enc_message[round] = chunk;
            ++round;
            memset(word, 0, sizeof(word));
        }
        word[i%WORD_SIZE] = s->sub[(int)message[i]];
    }

    size_t size = sizeof(NIGHT);

    fwrite(enc_mes, size, m_size, enc);
    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);

    n->file_char_length = m_size;

    free(message);
    fclose(f_to_enc);
    fclose(enc);
}

//-----------------------------------------------------------------------------
void decrypt_file(NIGHT *n){
    FILE *dcpt, *enc;
    enc = fopen(E_FILE, "rb");
    dcpt = fopen(D_FILE, "w");

    if( !enc ) perror("Error reading encrypted file."),exit(EXIT_FAILURE);
    if( !dcpt ) perror("Error opening decrypted file."),exit(EXIT_FAILURE);

    unsigned char enc_message[p->file_char_length];
    unsigned char message[p->file_char_length];
    size_t size = sizeof(unsigned char);

    fread(enc_message, size, p->file_char_length, enc);
    if( ferror(enc) )perror("Error reading encrypted file."),exit(EXIT_FAILURE);

    //++++++++++++++++++
    // Decrypt
    //++++++++++++++++++

    fclose(dcpt);
    fclose(enc);
}
#include "nightgale_c.h"


//-----------------------------------------------------------------------------
void encrypt_file(NIGHT *n, const char* file){
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
    if (nread != filesize) {
	printf("Error: reading input file...\n");
	exit(1);
    }

    int m_size = filesize;

    // TODO: this shouldn't be necessary - we may be encoding raw data, so we aren't using string conventions
    // message[m_size] = (unsigned char)'\0';

    unsigned char enc_message[m_size];

    printf("\nOriginal Message:\n");
    printf("%s\n", message);

    m_size -= 1;

    printf("Encrypted Message:\n");
    for(int i = 0; i < m_size; ++i){
        enc_message[i] = p->map1b[(unsigned int)message[i]];
        printf("%c", enc_message[i]);
    }
    printf("\n\n");

    size_t size = sizeof(unsigned char);

    fwrite(enc_message, size, m_size, enc);
    if( ferror(enc) ) perror("Error writing to encrypted file."),
                        exit(EXIT_FAILURE);

    p->file_char_length = m_size;

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

    printf("Message Decrypted:\n");
    for(int i = 0; i < p->file_char_length; ++i){
        message[i] = p->map1c[(unsigned int)enc_message[i]];
        printf("%c", message[i]);
    }
    printf("\n\n");

    fclose(dcpt);
    fclose(enc);
}
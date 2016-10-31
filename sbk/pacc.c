#include "pacc.h"

//-----------------------------------------------------------------------------
void encode(PACC *p){
    const unsigned char Chars1[PACC_SIZE] = {

        '\0', ' ', ',', '.',
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R',
        'S', 'T', 'U', 'V',
        'W', 'Y', 'a', 'b',
        'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r',
        's', 't', 'u', 'v',
        'w', 'x', 'y', 'z'
    };

    const unsigned char Bytes1[PACC_SIZE] = {
        0x0F, 0x17, 0x1B, 0x1D,
        0x1E, 0x27, 0x2B, 0x2D,
        0x2E, 0x33, 0x35, 0x36,
        0x39, 0x3A, 0x3C, 0x47,
        0x4B, 0x4D, 0x4E, 0x53,
        0x55, 0x56, 0x59, 0x5A,
        0x5C, 0x63, 0x65, 0x66,
        0x69, 0x6A, 0x6C, 0x71,
        0x72, 0x74, 0x78, 0x87,
        0x8B, 0x8D, 0x8E, 0x93,
        0x95, 0x96, 0x99, 0x9A,
        0x9C, 0xA3, 0xA5, 0xA6,
        0xA9, 0xAA, 0xAC, 0xB1,
        0xB2, 0xB4, 0xB8, 0xC3,
        0xC5, 0xC6, 0xC9, 0xCA,
        0xCC, 0xD1, 0xD2, 0xD4
    };

    for(int i = 0; i < PACC_SIZE; ++i){
        p->map1b[(int)Chars1[i]] = Bytes1[i];
        p->map1c[(int)Bytes1[i]] = Chars1[i];
    }
}

long long int get_file_length(FILE *fp) {
  fseek(fp, 0L, SEEK_CUR);
  long long int mypos = ftell(fp);
  fseek(fp, 0L, SEEK_END);
  long long int filesize = ftell(fp);
  fseek(fp, mypos, SEEK_SET);
  return filesize;
}

//-----------------------------------------------------------------------------
void encrypt_file(PACC *p, const char* file){
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
void decrypt_file(PACC *p){
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

//-----------------------------------------------------------------------------
void write_pacc(PACC *p){
    FILE *fp = fopen(PACC_KEY_NAME, "wb");
    if( !fp ) perror("Error opening PACC file."),exit(EXIT_FAILURE);

    size_t size = sizeof(*p);

    fwrite(p, size, 1, fp);
    if( ferror(fp) ) perror("Error writing to PACC file."),exit(EXIT_FAILURE);

    fclose(fp);
}

//-----------------------------------------------------------------------------
void read_pacc(PACC *p){
    FILE *fp = fopen(PACC_KEY_NAME, "rb");
    if( !fp ) perror("Error reading PACC file."),exit(EXIT_FAILURE);

    size_t size = sizeof(*p);

    fread(p, size, 1, fp);
    if( ferror(fp) ) perror("Error reading PACC file."),exit(EXIT_FAILURE);

    fclose(fp);
}

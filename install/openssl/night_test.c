#include <string.h>
#include <assert.h>
#include <openssl/nightgale.h>
#include <stdlib.h>
#include <sys/time.h>

#define RSA_KEY "private.pem"


double mysecond()
{
    struct timeval tp;
    gettimeofday(&tp, (void *)NULL);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


void system_test(){
    printf("1GB random check\n");
    size_t length = 1024 * 1024 * 1024;

    printf("Creating 1GB random buffer...\n");
    unsigned char *plain = calloc(sizeof(unsigned char), length);
    unsigned char *enc = calloc(sizeof(unsigned char), length);
    unsigned char *dec = calloc(sizeof(unsigned char), length);

    FILE *fp; 
    fp = fopen("/dev/urandom", "r");
    fread(plain, 1, length, fp);
    fclose(fp);

    SUB s_enc;
    nightgale_enc_set_key(&s_enc);

    printf("Encrypting...\n");

    double t1;
    t1 = mysecond();
    encrypt_night(&s_enc, length, plain, enc);
    t1 = mysecond() - t1;
    double rate = (((double)length)/1000000000.)/t1;

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    SUB s_dec;
    nightgale_dec_set_key(&s_dec);

    decrypt_night(&s_dec, length, enc, dec);

    int check = memcmp( plain, dec, length );       assert(check == 0);

    check = memcmp( plain, enc, length );       assert(check != 0);

    FILE *f = fopen("encrypt7.bin", "wb");
    fwrite(enc, length, 1, f);
    fclose(f);

    free(plain);
    free(enc);
    free(dec);
}


int main(){
    system_test();
    return 0;
}


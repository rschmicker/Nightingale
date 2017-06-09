#include "sub_t.h"
#include "nightgale_c.h"
#include "mysecond.h"
#include <assert.h>

int main(){

    printf("1GB random check\n");
    size_t length = 1024 * 1024 * 1024;

    printf("Creating 1GB random buffer...\n");
    unsigned char *plain = (unsigned char*)calloc(sizeof(unsigned char), length);
    unsigned char *enc = (unsigned char*)calloc(sizeof(unsigned char), length);
    unsigned char *dec = (unsigned char*)calloc(sizeof(unsigned char), length);
    
    FILE *fp;
    fp = fopen("/dev/urandom", "r");
    size_t i = fread(plain, 1, length, fp);
    if(i != length) {printf("Error reading from /dev/urandom\n"); exit(1);}
    fclose(fp);

    SUB s_enc;
    nightgale_enc_set_key(&s_enc);

    printf("Encrypting....\n");
    double t1;
    t1 = mysecond();
    encrypt_night(&s_enc, length, plain, enc);
    t1 = mysecond() - t1;
    double rate = (((double)length)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    SUB s_dec;
    nightgale_dec_set_key(&s_dec);

    decrypt_night(&s_dec, length, enc, dec);

    int check = memcmp( plain, dec, length );      assert(check == 0);
    check = memcmp( plain, enc, length );	   assert(check != 0);    

    printf("Pass!\n");

    free(plain);
    free(enc);
    free(dec);
    printf("==============================================================\n");

    return 0;
}

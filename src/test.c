#include "sub_t.h"
#include "nightgale.h"
#include "nightgale_p.h"
#include "mysecond.h"
#include <assert.h>

int main(){

    printf("1GB random check\n");
    size_t length = 1024 * 1024 * 1024;

    printf("Creating 1GB random buffer...\n");
    unsigned char *plain = (unsigned char*)calloc(sizeof(unsigned char), length);
    unsigned char *enc = (unsigned char*)calloc(sizeof(unsigned char), length);
    unsigned char *dec = (unsigned char*)calloc(sizeof(unsigned char), length);
    
    uint64_t *temp = (uint64_t *)plain;
    pcg64_random_t rng;
    pcg64_srandom_r(&rng, 42u, 54u);
    for(size_t i = 0; i < length/8; ++i)
	temp[i] = pcg64_random_r(&rng);

    double t1;
    double rate;
    int check;

    SUB s_enc;
    nightgale_enc_set_key(&s_enc);

    printf("Encrypting CBC-Like....\n");
    t1 = mysecond();
    encrypt_night(&s_enc, length, plain, enc);
    t1 = mysecond() - t1;
    rate = (((double)length)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time CBC-Like:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    SUB s_dec;
    nightgale_dec_set_key(&s_dec);

    decrypt_night(&s_dec, length, enc, dec);

    check = memcmp( plain, dec, length );      assert(check == 0);
    check = memcmp( plain, enc, length );	   assert(check != 0);    

    printf("CBC-Like Pass!\n");
    printf("---------------------------------------------------------------\n");
    //-------------------------------------------------------------------------
    
    printf("Encrypting Parallel....\n");

    SUB s_enc_p;
    nightgale_enc_set_key(&s_enc_p);
    
    t1 = mysecond();
    encrypt_night_p(&s_enc_p, length, plain, enc);
    t1 = mysecond() - t1;
    rate = (((double)length)/1000000000.)/t1;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time Parallel:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    SUB s_dec_p;
    nightgale_dec_set_key(&s_dec_p);

    decrypt_night_p(&s_dec_p, length, enc, dec);

    check = memcmp( plain, dec, length );      assert(check == 0);
    check = memcmp( plain, enc, length );      assert(check != 0);

    printf("Parallel Pass!\n");

    free(plain);
    free(enc);
    free(dec);
    printf("==============================================================\n");

    return 0;
}

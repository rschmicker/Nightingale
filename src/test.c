#include "sub_t.h"
#include "nightgale.h"
#include "mysecond.h"
#include "gmac.h"
#include <assert.h>

int main(){

    printf("1GB random check\n");
    size_t length = 1024 * 1024 * 1024;
    size_t align = 32;
    printf("Creating 1GB random buffer...\n");
    void *plain_alloc, *enc_alloc, *dec_alloc;
    posix_memalign(&plain_alloc, align, length);
    posix_memalign(&enc_alloc, align, length);
    posix_memalign(&dec_alloc, align, length);
    unsigned char *plain = (unsigned char *)plain_alloc;
    unsigned char *enc = (unsigned char *)enc_alloc;
    unsigned char *dec = (unsigned char *)dec_alloc;

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

    t1 = mysecond();

    check = memcmp( plain, dec, length );      assert(check == 0);
    check = memcmp( plain, enc, length );      assert(check != 0);

    printf("Parallel Pass!\n");

    //-------------------------------------------------------------------------

    printf("Encrypting Vectorised Parallel....\n");

    SUB_V s_enc_pv;
    nightgale_enc_set_key_v(&s_enc_pv);
    unsigned char tag_before[16], tag_after[16], iv[12];
    memcpy(iv, s_enc_p.seed4, 12);

    t1 = mysecond();
    encrypt_night_pv(&s_enc_pv, length, plain, enc);
    calc_gmac(plain, tag_before, length, s_enc_p.seed3, iv);
    t1 = mysecond() - t1;
    rate = (((double)length)/1000000000.)/t1;

    printf("GMAC Before:\n  ");
    for(size_t i = 0; i < sizeof(tag_before); i++)
    {
        printf("%02x", tag_before[i]);

        if(i == sizeof(tag_before) - 1) {
            printf("\n");
        }
    }

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Encrypt Time Vec Parallel:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    SUB_V s_dec_pv;
    nightgale_dec_set_key_v(&s_dec_pv);

    decrypt_night_pv(&s_dec_pv, length, enc, dec);
    calc_gmac(dec, tag_after, length, s_enc_p.seed3, iv);

    printf("GMAC After:\n  ");
    for(size_t i = 0; i < sizeof(tag_after); i++)
    {
        printf("%02x", tag_after[i]);

        if(i == sizeof(tag_after) - 1) {
            printf("\n");
        }
    }

    check = memcmp( plain, dec, length );      assert(check == 0);
    check = memcmp( plain, enc, length );      assert(check != 0);
    check = memcmp( tag_before, tag_after, 16 );      assert(check == 0);

    printf("Vectorised Parallel Pass!\n");

    free(plain);
    free(enc);
    free(dec);
    printf("==============================================================\n");

    return 0;
}

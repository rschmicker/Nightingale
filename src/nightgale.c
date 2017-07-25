#include "nightgale.h"

//-----------------------------------------------------------------------------
void encrypt_night(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out){

    size_t word_count = len / WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    uint64_t *enc_message = (uint64_t *)out;
    uint64_t *plain_text = (uint64_t *)in;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique, rng_anch, rng_ham;
    pcg128_t s1_unique, anchor_seed, ham_seed;

    temp = s->seed1;
    s1_unique = *(pcg128_t *)temp;

    temp = &s->digest[0];
    anchor_seed = *(pcg128_t *)temp;

    temp = &s->digest[SHA256_DIGEST_LENGTH/2];
    ham_seed = *(pcg128_t *)temp;

    pcg64_srandom_r(&rng_unique, s1_unique, 5);
    pcg64_srandom_r(&rng_anch, anchor_seed, 6);
    pcg64_srandom_r(&rng_ham, ham_seed, 7);

    // Anchor must call the PNRG first
    uint64_t anchor = pcg64_random_r(&rng_anch), root;
    uint64_t hamming_mask = pcg64_random_r(&rng_ham);

    unsigned char *pre_sub;
    uint64_t decimal_word;

    // Encrypt buffers
    root = anchor;
    for(size_t i = 0; i < word_count; ++i){
        decimal_word = root ^ plain_text[i] ^ hamming_mask;
        pre_sub = (unsigned char *)&decimal_word;
        for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = s->sub[(int)pre_sub[k]];
        uint64_t key = pcg64_random_r(&rng_unique);
        anchor = rotr64(anchor, key&MASK);
        enc_message[i] = decimal_word ^ key ^ anchor;
	root = decimal_word;
    }
 }

//-----------------------------------------------------------------------------
void decrypt_night(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out){

    size_t word_count = len / WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    uint64_t *enc_message = (uint64_t *)in;
    uint64_t *dec_message = (uint64_t *)out;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique, rng_anch, rng_ham;
    pcg128_t s1_unique, anchor_seed, ham_seed;

    temp = s->seed1;
    s1_unique = *(pcg128_t *)temp;

    temp = &s->digest[0];
    anchor_seed = *(pcg128_t *)temp;

    temp = &s->digest[SHA256_DIGEST_LENGTH/2];
    ham_seed = *(pcg128_t *)temp;

    pcg64_srandom_r(&rng_unique, s1_unique, 5);
    pcg64_srandom_r(&rng_anch, anchor_seed, 6);
    pcg64_srandom_r(&rng_ham, ham_seed, 7);

    // Anchor must call the PNRG first
    uint64_t anchor = pcg64_random_r(&rng_anch), root;
    uint64_t hamming_mask = pcg64_random_r(&rng_ham);

    unsigned char *pre_sub;
    uint64_t decimal_word, pre_sub_decimal_word;

    // Decrypt here
    root = anchor;
    for(size_t i = 0; i < word_count; ++i) {
	uint64_t key = pcg64_random_r(&rng_unique);
        anchor = rotr64(anchor, key&MASK);
        decimal_word = enc_message[i] ^ key ^ anchor;
		pre_sub_decimal_word = decimal_word;
        pre_sub = (unsigned char *)&decimal_word;
        for(int j = 0; j < WORD_SIZE; ++j) pre_sub[j] = s->reverse_sub[(int)pre_sub[j]];
        dec_message[i] = root ^ decimal_word ^ hamming_mask;
        root = pre_sub_decimal_word;
    }
}

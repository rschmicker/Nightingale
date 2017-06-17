#include "nightgale_p.h"

#define THREAD_COUNT 3

//-----------------------------------------------------------------------------
void encrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out){

    size_t word_count = len / WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    uint64_t *plain_text = (uint64_t*)in;
    uint64_t *encrypted_text = (uint64_t*)out;

    size_t num_threads = THREAD_COUNT;
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;

    pthread_t threads[num_threads];
    
    // Encrypt buffers
    for(size_t i = 0; i < num_threads; ++i){
        thread_data td;
	int division_size = word_count/num_threads;
	int offset = i * division_size;
	td.in = &plain_text[offset];
	td.out = &encrypted_text[offset];
	td.td_word_count = td_word_count;
	td.s = s;
	pthread_create(&threads[i], NULL, encrypt, (void *)&td);
    }
    for(size_t k = 0; k < num_threads; ++k)
	    pthread_join(threads[k], NULL);
 }

//-----------------------------------------------------------------------------
void* encrypt(void *t){
    thread_data *td = (thread_data*)t;
   

    // reduce size in test.c to 8 bytes
    // print out each step of the way

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;
    pcg64_srandom_r(&rng_unique, s1_unique, 5);

    uint64_t decimal_word;
    unsigned char *pre_sub;

    for(size_t i = 0; i < td->td_word_count; ++i){
	decimal_word = td->in[i] ^ pcg64_random_r(&rng_unique);
	pre_sub = (unsigned char *)&decimal_word;
	for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = td->s->sub[(int)pre_sub[k]];
	td->out[i] = decimal_word ^ pcg64_random_r(&rng_unique);
    }   

    return NULL;
}

//-----------------------------------------------------------------------------
void decrypt_night_p(SUB *s, size_t len, const unsigned char *in, 
			unsigned char *out){

    size_t word_count = len / WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    uint64_t *encrypted_text = (uint64_t*)in;
    uint64_t *decrypted_text = (uint64_t*)out;

    size_t num_threads = THREAD_COUNT;
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;

    pthread_t threads[num_threads];
		    
    // Decrypt here
    for(size_t i = 0; i < num_threads; ++i){
	thread_data td;
	int division_size = word_count/num_threads;
	int offset = i * division_size;
	td.in = &encrypted_text[offset];
	td.out = &decrypted_text[offset];
	td.td_word_count = td_word_count;
	td.s = s;
	pthread_create(&threads[i], NULL, decrypt, (void *)&td);
    }
    for(size_t k = 0; k < num_threads; ++k)
	pthread_join(threads[k], NULL);
}

//-----------------------------------------------------------------------------
void* decrypt(void *t){
    thread_data *td = (thread_data*)t;
    
    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;
    pcg64_srandom_r(&rng_unique, s1_unique, 5);
    
    uint64_t decimal_word;
    unsigned char *pre_sub;
    
    for(size_t i = 0; i < td->td_word_count; ++i){
    	uint64_t key2 = pcg64_random_r(&rng_unique);
	uint64_t key1 = pcg64_random_r(&rng_unique);	
	decimal_word = td->in[i] ^ key1;
	pre_sub = (unsigned char *)&decimal_word;
	for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = td->s->reverse_sub[(int)pre_sub[k]];
	td->out[i] = decimal_word ^ key2;
    }

    return NULL;
}


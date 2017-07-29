#include "nightgale.h"

//-----------------------------------------------------------------------------
void encrypt_night_p(SUB *s, size_t len, const unsigned char *in,
			unsigned char *out){

    size_t word_count = len / WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    uint64_t *plain_text = (uint64_t*)in;
    uint64_t *encrypted_text = (uint64_t*)out;

    size_t num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    // if( (num_threads & (num_threads - 1)) != 0 )
	// 	num_threads = round_power_down(num_threads);
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;
	bool last_thread_extra_word = false;
	float word_count_checker = (float)word_count / (float)num_threads;
	if(word_count_checker - (int)word_count_checker != 0)
		last_thread_extra_word = true;

    pthread_t threads[num_threads];
    thread_data *contexts[num_threads];
    for(size_t i = 0; i < num_threads; ++i){
		contexts[i] = (thread_data *)calloc(1, sizeof(thread_data));
    }
    // Encrypt buffers
    for(size_t i = 0; i < num_threads; ++i){
		int division_size = word_count/num_threads;
		int offset = i * division_size;
		contexts[i]->in = &plain_text[offset];
		contexts[i]->out = &encrypted_text[offset];
		contexts[i]->td_word_count = td_word_count;
		if(i == (1 - num_threads) && last_thread_extra_word)
			contexts[i]->td_word_count++;
		contexts[i]->s = s;
		contexts[i]->stream_num = offset;
		pthread_create(&(threads[i]), NULL, encrypt_threaded, (void *)contexts[i]);
    }
    for(size_t k = 0; k < num_threads; ++k)
	    pthread_join(threads[k], NULL);
 }

//-----------------------------------------------------------------------------
void* encrypt_threaded(void *t){
    thread_data *td = (thread_data*)t;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;

    uint64_t decimal_word;
    unsigned char *pre_sub;

    for(size_t i = 0; i < td->td_word_count; ++i){
        pcg64_srandom_r(&rng_unique, s1_unique, td->stream_num+i);
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

    size_t num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    if( (num_threads & (num_threads - 1)) != 0 )
        num_threads = round_power_down(num_threads);
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;

    pthread_t threads[num_threads];
    thread_data *contexts[num_threads];
    for(size_t i = 0; i < num_threads; ++i){
		contexts[i] = (thread_data *)calloc(1, sizeof(thread_data));
    }
    // Decrypt here
    for(size_t i = 0; i < num_threads; ++i){
		int division_size = word_count/num_threads;
		int offset = i * division_size;
		contexts[i]->in = &encrypted_text[offset];
		contexts[i]->out = &decrypted_text[offset];
		contexts[i]->td_word_count = td_word_count;
		contexts[i]->s = s;
		contexts[i]->stream_num = offset;
		pthread_create(&(threads[i]), NULL, decrypt_threaded, (void *)contexts[i]);
    }
    for(size_t k = 0; k < num_threads; ++k)
	pthread_join(threads[k], NULL);
}

//-----------------------------------------------------------------------------
void* decrypt_threaded(void *t){
    thread_data *td = (thread_data*)t;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;

    uint64_t decimal_word;
    unsigned char *pre_sub;

    for(size_t i = 0; i < td->td_word_count; ++i){
        pcg64_srandom_r(&rng_unique, s1_unique, td->stream_num+i);
    	uint64_t key2 = pcg64_random_r(&rng_unique);
		uint64_t key1 = pcg64_random_r(&rng_unique);
		decimal_word = td->in[i] ^ key1;
		pre_sub = (unsigned char *)&decimal_word;
		for(int k = 0; k < WORD_SIZE; ++k) pre_sub[k] = td->s->reverse_sub[(int)pre_sub[k]];
		td->out[i] = decimal_word ^ key2;
    }

    return NULL;
}

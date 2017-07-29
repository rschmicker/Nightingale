#include "nightgale.h"

void print256_num(__m256i var, char* name){
	uint8_t *val = (uint8_t*) &var;
	printf("%s: ", name);
	int i;
	for(i = 0; i < 32; ++i){
		printf("%d, ", (int)val[i]);
	}
	printf("\n");
}

//-----------------------------------------------------------------------------
void encrypt_night_pv(SUB_V *s, size_t len, const unsigned char *in,
			unsigned char *out){

    size_t word_count = len / VECTOR_WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    __m256i *plain_text = (__m256i *)in;
    __m256i *encrypted_text = (__m256i *)out;

    size_t num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;
    size_t last_thread_extra_count = 0;
    float word_count_checker = (float)word_count / (float)num_threads;
    if(word_count_checker - (int)word_count_checker != 0){
            last_thread_extra_count = word_count % num_threads;
    }

    pthread_t threads[num_threads];
    thread_vec_data *contexts[num_threads];
    for(size_t i = 0; i < num_threads; ++i){
	    thread_vec_data *td = (thread_vec_data *)calloc(1, sizeof(thread_vec_data));
        contexts[i] = td;
    }
    // Encrypt buffers
    for(size_t i = 0; i < num_threads; ++i){
    	int offset = i * (word_count/num_threads);
    	contexts[i]->in = &plain_text[offset];
    	contexts[i]->out = &encrypted_text[offset];
    	contexts[i]->td_word_count = td_word_count;
    	if(i == (num_threads - 1) && last_thread_extra_count != 0){
            contexts[i]->td_word_count += last_thread_extra_count;
        }
	contexts[i]->s = s;
    	contexts[i]->stream_num = offset;
    	pthread_create(&(threads[i]), NULL, encrypt_threaded_v, (void *)contexts[i]);
    }
    for(size_t k = 0; k < num_threads; ++k)
	    pthread_join(threads[k], NULL);
}


//-----------------------------------------------------------------------------
void* encrypt_threaded_v(void *t){
    thread_vec_data *td = (thread_vec_data*)t;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;

    uint64_t rand_words[4];

    __m256i msg, msg_xor, msg_high, msg_low,
            out_high, out_low, out_msg, out_xor;

    __m256i sub_low = *(__m256i *)&td->s->sub_v_low;
    __m256i sub_high = *(__m256i *)&td->s->sub_v_high;
    __m256i mask_high = *(__m256i *)high_bytes;
	__m256i mask_low = *(__m256i *)low_bytes;

    for(size_t i = 0; i < td->td_word_count; ++i){
        pcg64_srandom_r(&rng_unique, s1_unique, td->stream_num+i);
        rand_words[0] = pcg64_random_r(&rng_unique);
    	rand_words[1] = pcg64_random_r(&rng_unique);
        rand_words[2] = pcg64_random_r(&rng_unique);
        rand_words[3] = pcg64_random_r(&rng_unique);
        msg = td->in[i];
        msg_xor = _mm256_xor_si256(msg, *(__m256i *)rand_words);
        msg_high = _mm256_and_si256(mask_low, msg_xor);
        msg_high = _mm256_srlv_epi32(msg_high, *(__m256i*)multi_factor_right);
        msg_low = _mm256_and_si256(mask_high, msg_xor);
        out_high = _mm256_shuffle_epi8(sub_high, msg_high);
        out_high = _mm256_sllv_epi32(out_high, *(__m256i*)multi_factor_right);
        out_low = _mm256_shuffle_epi8(sub_low, msg_low);
        out_msg = out_high + out_low;
        rand_words[0] = pcg64_random_r(&rng_unique);
    	rand_words[1] = pcg64_random_r(&rng_unique);
        rand_words[2] = pcg64_random_r(&rng_unique);
        rand_words[3] = pcg64_random_r(&rng_unique);
        out_xor = _mm256_xor_si256(out_msg, *(__m256i *)rand_words);
        td->out[i] = out_xor;
    }

    return NULL;
}


//-----------------------------------------------------------------------------
void decrypt_night_pv(SUB_V *s, size_t len, const unsigned char *in,
			unsigned char *out){
    size_t word_count = len / VECTOR_WORD_SIZE;
    if( len % WORD_SIZE != 0 ) ++word_count;

    __m256i *encrypted_text = (__m256i *)in;
    __m256i *decrypted_text = (__m256i *)out;

    size_t num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    if( word_count < num_threads ) num_threads = 1;
    size_t td_word_count = word_count / num_threads;
    size_t last_thread_extra_count = 0;
    float word_count_checker = (float)word_count / (float)num_threads;
    if(word_count_checker - (int)word_count_checker != 0){
            last_thread_extra_count = word_count % num_threads;
    }

    pthread_t threads[num_threads];
    thread_vec_data *contexts[num_threads];
    for(size_t i = 0; i < num_threads; ++i){
        thread_vec_data *td = (thread_vec_data *)calloc(1, sizeof(thread_vec_data));
        contexts[i] = td;
    }
    // Decrypt here
    for(size_t i = 0; i < num_threads; ++i){
        int offset = i * (word_count/num_threads);
        contexts[i]->in = &encrypted_text[offset];
        contexts[i]->out = &decrypted_text[offset];
        contexts[i]->td_word_count = td_word_count;
        if(i == (num_threads - 1) && last_thread_extra_count != 0){
            contexts[i]->td_word_count += last_thread_extra_count;
        }
	contexts[i]->s = s;
        contexts[i]->stream_num = offset;
        pthread_create(&(threads[i]), NULL, decrypt_threaded_v, (void *)contexts[i]);
    }
    for(size_t k = 0; k < num_threads; ++k)
    pthread_join(threads[k], NULL);
}


//-----------------------------------------------------------------------------
void* decrypt_threaded_v(void *t){
    thread_vec_data *td = (thread_vec_data*)t;

    // PNRG initialization
    void *temp;
    pcg64_random_t rng_unique;
    pcg128_t s1_unique;
    temp = td->s->seed1;
    s1_unique = *(pcg128_t *)temp;

    uint64_t rand_words_1[4];
    uint64_t rand_words_2[4];

    __m256i msg, msg_xor, msg_high, msg_low,
            out_high, out_low, out_msg, out_xor;

    __m256i sub_low = *(__m256i *)&td->s->r_sub_v_low;
    __m256i sub_high = *(__m256i *)&td->s->r_sub_v_high;
    __m256i mask_high = *(__m256i *)high_bytes;
	__m256i mask_low = *(__m256i *)low_bytes;

    for(size_t i = 0; i < td->td_word_count; ++i){
        pcg64_srandom_r(&rng_unique, s1_unique, td->stream_num+i);
        rand_words_2[0] = pcg64_random_r(&rng_unique);
    	rand_words_2[1] = pcg64_random_r(&rng_unique);
        rand_words_2[2] = pcg64_random_r(&rng_unique);
        rand_words_2[3] = pcg64_random_r(&rng_unique);
        rand_words_1[0] = pcg64_random_r(&rng_unique);
    	rand_words_1[1] = pcg64_random_r(&rng_unique);
        rand_words_1[2] = pcg64_random_r(&rng_unique);
        rand_words_1[3] = pcg64_random_r(&rng_unique);
        msg = td->in[i];
        msg_xor = _mm256_xor_si256(msg, *(__m256i *)rand_words_1);
        msg_low = _mm256_and_si256(mask_high, msg_xor);
        msg_high = _mm256_and_si256(mask_low, msg_xor);
        msg_high = _mm256_srlv_epi32(msg_high, *(__m256i*)multi_factor_right);
        out_high = _mm256_shuffle_epi8(sub_high, msg_high);
        out_high = _mm256_sllv_epi32(out_high, *(__m256i*)multi_factor_right);
        out_low = _mm256_shuffle_epi8(sub_low, msg_low);
        out_msg = out_high + out_low;
        out_xor = _mm256_xor_si256(out_msg, *(__m256i *)rand_words_2);
        td->out[i] = out_xor;
    }

    return NULL;
}

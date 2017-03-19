#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/nightgale.h>
#include "internal/cryptlib.h"
#include "internal/evp_int.h"

typedef struct {
	SUB s;
	union {
		void (*cipher) (SUB *s, size_t len, const unsigned char *in,
                                    unsigned char *out);
	} stream;
} EVP_NIGHT_KEY;

#define data(ctx) ((EVP_NIGHT_KEY *)EVP_CIPHER_CTX_get_cipher_data(ctx))

static int nightgale_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
				const unsigned char *iv, int enc);

static int nightgale_c(EVP_CIPHER_CTX *ctx, unsigned char *out,
				const unsigned char *in, size_t inl);

static const EVP_CIPHER nightgale_cipher = {
	NID_nightgale,
	1, 1, 0, // block_size, key size, iv size 
	0,
	nightgale_init_key, // function pointer to key init
	nightgale_c, // function pointer to cipher operation
	NULL, // clean up function pointer
	sizeof(EVP_NIGHT_KEY), // size of ctx
	NULL,
	NULL,
	NULL,
	NULL	
};

const EVP_CIPHER *EVP_nightgale(void)
{
	return (&nightgale_cipher);
}

static int nightgale_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                                const unsigned char *iv, int enc)
{
	enc ? nightgale_enc_set_key(&data(ctx)->s) : 
			nightgale_dec_set_key(&data(ctx)->s);
	data(ctx)->stream.cipher = enc ? encrypt_night : decrypt_night;
	return 1;
}

static int nightgale_c(EVP_CIPHER_CTX *ctx, unsigned char *out,
				const unsigned char *in, size_t inl)
{
	(*data(ctx)->stream.cipher) (&data(ctx)->s, inl, in, out);
	return 1;
}


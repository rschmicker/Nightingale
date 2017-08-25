#include "gmac.h"

void calc_gmac(unsigned char *aad, unsigned char *tag, size_t len,
                unsigned char *key, unsigned char *iv){
    // Calculate GMAC
	int unused = 0;
	EVP_CIPHER_CTX *ctx = NULL;
	ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL) {
        exit (1);
    }

	int rc = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    if(rc != 1) {
        exit (1);
    }

	rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL);
    if(rc != 1) {
        exit (1);
    }

	rc = EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);
    if(rc != 1) {
        exit (1);
    }

    rc = EVP_EncryptUpdate(ctx, NULL, &unused, aad, len);
    if(rc != 1) {
        exit (1);
    }

    rc = EVP_EncryptFinal_ex(ctx, NULL, &unused);
    if(rc != 1) {
        exit (1);
    }

    rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag);
    if(rc != 1) {
        exit (1);
    }

    if(ctx) {
        EVP_CIPHER_CTX_free(ctx);
    }
}

#pragma once
#include <openssl/evp.h>

#define IV_SIZE 12
#define TAG_SIZE 16

//-----------------------------------------------------------------------------
// Calculate the GMAC of a given message
//-----------------------------------------------------------------------------
void calc_gmac(unsigned char *aad, unsigned char *tag, size_t len,
                unsigned char *key, unsigned char *iv);

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <assert.h> 
#include <sys/time.h>


double mysecond()
{
    struct timeval tp;
    gettimeofday(&tp, (void *)NULL);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_nightgale(), NULL, key, iv))
    handleErrors();
  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;


  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;


  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_nightgale(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

int main (void)
{
  /* Set up the key and iv. Do I need to say to not hard code these in a
   * real application? :-)
   */

  int length = 1024*1024*1024;
  printf("Creating 1GB random buffer...\n");
  unsigned char* plaintext = calloc(sizeof(unsigned char), length);
  FILE *fp;
  fp = fopen("/dev/urandom", "r");
  fread(plaintext, 1, length, fp);
  fclose(fp);

  /* A 256 bit key */
  unsigned char *key = (unsigned char *)"0";

  /* A 128 bit IV */
  unsigned char *iv = (unsigned char *)"0";

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  unsigned char *ciphertext = calloc(sizeof(unsigned char), length+8);

  /* Buffer for the decrypted text */
  unsigned char *decryptedtext = calloc(sizeof(unsigned char), length+8);

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();

  printf("Encrypting....\n");
  // encrypt
  double t1;
  t1 = mysecond();

  /* Encrypt the plaintext */
  int ciphertext_len = encrypt (plaintext, length, key, iv, ciphertext);
  t1 = mysecond() - t1;
  double rate = (((double)length)/1000000000.)/t1;

  printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("Encrypt Time:\t%5.3fms\tRate:\t%5.3fGB/s\n", t1*1000., rate);
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

  /* Decrypt the ciphertext */
  decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

  int check;
  check = memcmp( plaintext, decryptedtext, length ); assert(check == 0);
  check = memcmp( plaintext, ciphertext, length ); assert(check != 0);

  /* Clean up */
  EVP_cleanup();
  ERR_free_strings();

  return 0;
}



# Nightingale

## Description
  - To be added

## Automated integration into OpenSSL
  - Run install.sh

## Manual integration into OpenSSL
  - This is helpful incase the structure of OpenSSL changes
    and the above script no longer works
  
  1. Download...
    * OpenSSL:      wget https://github.com/openssl/openssl/archive/master.zip
    * Nightingale:  wget https://github.com/rschmicker/Nightingale/archive/master.zip
  
  2. Move some files around...
    * Nightingale/openssl-migration/nightgale/  -> openssl/crypto/nightgale
    * Nightingale/openssl-migration/e\_night.c  -> openssl/crypto/evp/e\_night.c
    * Nightingale/openssl-migration/nightgale.h -> openssl/include/openssl/nightgale.h

  3. Time to edit some files...
    * crypto/evp/build.info to include compiling of e\_night.c
        ```
        e_night.c e_xcbc_d.c e_rc2.c e_cast.c e_rc5.c \
        ```
    * crypto/evp/c\_allc.c to include loading of cipher:
        ```C
        void openssl_add_all_ciphers_int(void)
        {
            EVP_add_cipher(EVP_nightgale());
            EVP_add_cipher_alias(SN_nightgale, "nightgale");
        
        #ifndef OPENSSL_NO_DES
            EVP_add_cipher(EVP_des_cfb());
            EVP_add_cipher(EVP_des_cfb1());
        ```
    * crypto/objects/objects.txt to include nightgale cipher around line 864:
        ```
        #Nightgale
                    : NIGHTGALE : nightgale
                        
        # AES aka Rijndael

        !Alias csor 2 16 840 1 101 3
        !Alias nistAlgorithms csor 4
        !Alias aes nistAlgorithms 1
        ```
    * Configure in openssl root directory to include nightgale in compilation around line 315:
        ```
        "md2", "md4", "md5", "nightgale", "sha",
        ```
    * util/mkdef.pl to include header file for nightgale around line 270:
        ```perl
        $crypto.=" include/openssl/nightgale.h";
        ```
    * include/openssl/evp.h to add in EVP\_nightgale() around line 727:
        ```C
        const EVP_CIPHER *EVP_nightgale(void);
        ```
  
  4. Let's give it a test!
    * move:
            Nightingale/openssl-migration/compile_night_test.sh 
            Nightingale/openssl-migration/night_evp_test.c 
            Nightingale/openssl-migration/night_test.c  
      to the root directory of openssl
    * make shell scripts excutable with chmod +x
    * run it!
        * the test creates a 1GB random buffer
        * then encrypt and decrypt the buffer
        * once finished it will compare the original buffer 
          to the decrypted buffer and make sure they are equal
        * and compare the original buffer to the encrypted buffer 
          and make sure they are not equal
        * the evp program uses the EVP interface and the latter 
          directly calls the nightgale functions
        * finally, a call to OpenSSL's speedtest is done                                                
  
  5. for completeness sake
    * run 'make test' in the root directory of openssl to 
      perform OpenSSL's unit test


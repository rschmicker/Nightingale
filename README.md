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
    * Move the following to the root directory of OpenSSL:
        * Nightingale/openssl-migration/compile_night_test.sh 
        * Nightingale/openssl-migration/night_evp_test.c 
        * Nightingale/openssl-migration/night_test.c  
    * Make the shell script excutable with chmod +x
    * Run it!
        * The test creates a 1GB random buffer
        * Then encrypts and decrypts the buffer
        * Once finished it will compare the original buffer 
          to the decrypted buffer and verify that they are equal
        * It also compares the original buffer to the encrypted buffer 
          to verify that they are not equal
        * The evp program uses the EVP interface and the latter 
          directly calls the nightgale functions
        * Finally, a call to OpenSSL's speedtest is done                                                
    * The final output of the script should be something like this...
        ```
        1GB random check
        Creating 1GB random buffer...
        Encrypting...
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Encrypt Time:   1319.684ms  Rate:   0.814GB/s
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Creating 1GB random buffer...
        Encrypting....
        +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Encrypt Time:   1419.162ms  Rate:   0.757GB/s
        +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        You have chosen to measure elapsed time instead of user CPU time.
        Doing nightgale for 3s on 16 size blocks: 82622879 nightgale's in 2.92s
        Doing nightgale for 3s on 64 size blocks: 34945044 nightgale's in 2.96s
        Doing nightgale for 3s on 256 size blocks: 10332545 nightgale's in 2.92s
        Doing nightgale for 3s on 1024 size blocks: 2708537 nightgale's in 2.97s
        Doing nightgale for 3s on 8192 size blocks: 346215 nightgale's in 2.96s
        Doing nightgale for 3s on 16384 size blocks: 173719 nightgale's in 2.96s
        OpenSSL 1.1.1-dev  xx XXX xxxx
        built on: reproducible build, date unspecified
        options:bn(64,64) rc4(16x,int) des(int) aes(partial) idea(int) blowfish(ptr) 
        compiler: gcc -DDSO_DLFCN -DHAVE_DLFCN_H -DNDEBUG -DOPENSSL_THREADS -DOPENSSL_NO_STATIC_ENGINE 
        -DOPENSSL_PIC -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 
        -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DRC4_ASM -DMD5_ASM 
        -DAES_ASM -DVPAES_ASM -DBSAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DPADLOCK_ASM 
        -DPOLY1305_ASM -DOPENSSLDIR="\"/usr/local/ssl\"" -DENGINESDIR="\"/usr/local/lib/engines-1.1\""  
        -Wa,--noexecstack
        The 'numbers' are in 1000s of bytes per second processed.
        type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
        nightgale       452728.10k   755568.52k   905866.96k   933852.49k   958173.41k   961558.14k
        ```
  5. For completeness sake
    * Run 'make test' in the root directory of openssl to 
      perform OpenSSL's unit test


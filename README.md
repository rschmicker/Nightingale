# Nightingale

## Description
  - To be added

## (Preferred Method) Automated integration into OpenSSL
  - Run install.sh
    * Downloads Openssl
    * Applies a patch file to add in all Nightingale components
    * Runs make for openssl
    * Runs a couple test programs and OpenSSL's speedtest
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

## (Alternative Method) Manual integration into OpenSSL
  - This is helpful incase the structure of OpenSSL changes
    and the above script no longer works
  
  1. Download...
    * OpenSSL:      wget https://github.com/openssl/openssl/archive/master.zip
    * Nightingale:  wget https://github.com/rschmicker/Nightingale/archive/master.zip
  
  2. Move some files around...
```
cp -r Nightingale/openssl-migration/nightgale/ openssl/crypto/nightgale
cp Nightingale/openssl-migration/e_night.c openssl/crypto/evp/e_night.c
cp Nightingale/openssl-migration/nightgale.h openssl/include/openssl/nightgale.h
```
  3. Time to edit some files...
    * openssl/crypto/evp/build.info to include compiling of e\_night.c
        ```
        e_night.c e_xcbc_d.c e_rc2.c e_cast.c e_rc5.c \
        ```
    * openssl/crypto/evp/c\_allc.c to include loading of cipher:
        ```C
        void openssl_add_all_ciphers_int(void)
        {
            EVP_add_cipher(EVP_nightgale());
            EVP_add_cipher_alias(SN_nightgale, "nightgale");
        
        #ifndef OPENSSL_NO_DES
            EVP_add_cipher(EVP_des_cfb());
            EVP_add_cipher(EVP_des_cfb1());
        ```
    * openssl/crypto/objects/objects.txt to include nightgale cipher around line 864:
        ```
        #Nightgale
                    : NIGHTGALE : nightgale
                        
        # AES aka Rijndael

        !Alias csor 2 16 840 1 101 3
        !Alias nistAlgorithms csor 4
        !Alias aes nistAlgorithms 1
        ```
    * openssl/Configure to include nightgale in compilation around line 315:
        ```
        "md2", "md4", "md5", "nightgale", "sha",
        ```
    * openssl/util/mkdef.pl to include header file for nightgale around line 270:
        ```perl
        $crypto.=" include/openssl/nightgale.h";
        ```
    * openssl/include/openssl/evp.h to add in EVP\_nightgale() around line 727:
        ```C
        const EVP_CIPHER *EVP_nightgale(void);
        ```
    * openssl/ssl/ssl\_ciph.c to begin integrating Nightingale around line 71:
        ```C
        #define SSL_ENC_CHACHA_IDX      19
        #define SSL_ENC_NIGHTGALE       20
        #define SSL_ENC_NUM_IDX         21
        ``` 
    * openssl/ssl/ssl\_ciph.c to add mapping of SSL\_NIGHTGALE to its NID around line 103:
        ```C
        {SSL_CHACHA20POLY1305, NID_chacha20_poly1305},
        {SSL_NIGHTGALE, NID_nightgale},
        };
        ```

    * openssl/ssl/ssl\_ciph.c to include Nightingale's alias around line 301:
        ```C
        {0, SSL_TXT_CHACHA20, 0, 0, 0, SSL_CHACHA20},
        {0, SSL_TXT_NIGHTGALE, 0, 0, 0, SSL_NIGHTGALE},
        ``` 
 
    * openssl/ssl/ssl\_ciph.c to add Nightingale's cipher description around line 1669:
        ```C
        case SSL_CHACHA20POLY1305:
            enc = "CHACHA20/POLY1305(256)";
            break;
        case SSL_NIGHTGALE:
            enc = "NIGHTGALE";
            break;
        default:
            enc = "unknown";
            break;
        }
        ```

    * openssl/ssl/ssl\_init.c to load the EVP instance around line 32:
        ```C
        static int ssl_base_inited = 0;
        DEFINE_RUN_ONCE_STATIC(ossl_init_ssl_base)
        {
        #ifdef OPENSSL_INIT_DEBUG
            fprintf(stderr, "OPENSSL_INIT: ossl_init_ssl_base: "
                    "Adding SSL ciphers and digests\n");
        #endif

            EVP_add_cipher(EVP_nightgale());

        #ifndef OPENSSL_NO_DES
            EVP_add_cipher(EVP_des_cbc());
            EVP_add_cipher(EVP_des_ede3_cbc());
        ```

    * openssl/include/openssl/ssl.h to create the cipher's SSL macro around line 160:
        ```C
        # define SSL_TXT_GOST            "GOST89"

        # define SSL_TXT_NIGHTGALE       "NIGHTGALE"

        # define SSL_TXT_MD5             "MD5"
        ```

    * openssl/include/openssl/tls1.h to define Nightingale's CK macro around line 605:
        ```C
        # define TLS1_CK_ECDHE_PSK_WITH_NULL_SHA384              0x0300C03B

        /* Nightingale ciphersuite */
        # define TLS1_CK_ECDHE_RSA_WITH_NIGHTGALE_SHA384         0x0300C03C

        /* Camellia-CBC ciphersuites from RFC6367 */
        # define TLS1_CK_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 0x0300C072
        ```

    * openssl/include/openssl/tls1.h to define Nightgale's TXT macro around line 739:
        ```C
        # define TLS1_TXT_RSA_PSK_WITH_NULL_SHA384               "RSA-PSK-NULL-SHA384"

        /* Nightgale ciphersuite */
        # define TLS1_TXT_ECDHE_RSA_WITH_NIGHTGALE_SHA384        "ECDHE-RSA-NIGHTGALE-SHA384"

        /* SRP ciphersuite from RFC 5054 */
        # define TLS1_TXT_SRP_SHA_WITH_3DES_EDE_CBC_SHA          "SRP-3DES-EDE-CBC-SHA"
        ```

    * openssl/ssl/s3\_lib.c to add the heart of the ciphersuite around line 1232:
        ```C
        {
        1,
        TLS1_TXT_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
        TLS1_CK_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
        SSL_kECDHE,
        SSL_aRSA,
        SSL_AES256GCM,
        SSL_AEAD,
        TLS1_2_VERSION, TLS1_2_VERSION,
        DTLS1_2_VERSION, DTLS1_2_VERSION,
        SSL_HIGH | SSL_FIPS,
        SSL_HANDSHAKE_MAC_SHA384 | TLS1_PRF_SHA384,
        256,
        256,
        },
        {
        1,
        TLS1_TXT_ECDHE_RSA_WITH_NIGHTGALE_SHA384,
        TLS1_CK_ECDHE_RSA_WITH_NIGHTGALE_SHA384,
        SSL_kECDHE,
        SSL_aRSA,
        SSL_NIGHTGALE,
        SSL_SHA384,
        TLS1_VERSION, TLS1_2_VERSION,
        DTLS1_VERSION, DTLS1_2_VERSION,
        SSL_HIGH | SSL_FIPS,
        SSL_HANDSHAKE_MAC_SHA384 | TLS1_PRF_SHA384,
        512,
        512,
        },

        #endif                          /* OPENSSL_NO_EC */
        ```

    * openssl/ssl/ssl\_locl.h to define Nightingale's SSL macro around line 261:
        ```C
        # define SSL_CHACHA20POLY1305    0x00080000U
        # define SSL_NIGHTGALE           0x00100000U

        # define SSL_AESGCM              (SSL_AES128GCM | SSL_AES256GCM)
        ```

  4. Let's give it a test!
    * Move the test files into the root directory of openssl:
      ```
      cp Nightingale/openssl-migration/compile_night_test.sh openssl/
      cp Nightingale/openssl-migration/night_evp_test.c openssl/
      cp Nightingale/openssl-migration/night_test.c openssl/
      ```
    * Make the shell script excutable
      ```
      chmod +x Nightingale/openssl-migration/compile_night_test.sh
      ```
    * Run Nightingale/openssl-migration/compile\_night\_test.sh
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

    * To test the integrated ciphersuite into libssl do the following:
        * Open two terminals and navigate both to the pingpong directory in Nightingale's source
        * Run the following to compile the example SSL programs:
            ```
            ./compile.sh
            ```
        * In one terminal start the server with the following (password is hello):
            ```
            LD_LIBRARY_PATH=<compiled openssl dir> ./server 5000
            ```
        * In the second terminal launch the client to connect to the server:
            ```
            LD_LIBRARY_PATH=<compiled openssl dir> ./client 5000
            ```
        * The output of the server should be the following:
            ```
            Enter PEM pass phrase:
            Connection: 127.0.0.1:42830
            Server certificates:
            Subject: /C=AU/ST=Some-State/O=Internet Widgits Pty Ltd
            Issuer: /C=AU/ST=Some-State/O=Internet Widgits Pty Ltd
            Client msg: "Hello???"
            ```
        * And the output of the client should be:
            ```
            Enter PEM pass phrase:
            Connected with ECDHE-RSA-NIGHTGALE-SHA384 encryption
            Server certificates:
            Subject: /C=AU/ST=Some-State/O=Internet Widgits Pty Ltd
            Issuer: /C=AU/ST=Some-State/O=Internet Widgits Pty Ltd
            Received: "<html><body><pre>Hello???</pre></body></html>
            
            "
            ```

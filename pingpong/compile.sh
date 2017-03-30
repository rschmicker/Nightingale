gcc server.c -o server -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc client.c -o client -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl

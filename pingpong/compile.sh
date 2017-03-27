gcc server.c -o server -I../install/openssl-master/include -L../install/openssl-master/ -lcrypto -lssl
gcc client.c -o client -I../install/openssl-master/include -L../install/openssl-master/ -lcrypto -lssl

gcc server.c -o server -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc client.c -o client -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc server_speed_night.c -o server_speed_night -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc client_speed_night.c -o client_speed_night -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc server_speed_aes.c -o server_speed_aes -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
gcc client_speed_aes.c -o client_speed_aes -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl


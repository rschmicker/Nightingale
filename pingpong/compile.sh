CC=gcc

$CC server.c -o server -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
$CC client.c -o client -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
$CC server_speed_night.c -o server_speed_night -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
$CC client_speed_night.c -o client_speed_night -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
$CC server_speed_aes.c -o server_speed_aes -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl
$CC client_speed_aes.c -o client_speed_aes -I../install/openssl/include -L../install/openssl/ -lcrypto -lssl


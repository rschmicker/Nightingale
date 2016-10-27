gcc -std=c99 -I/usr/local/openssl/include/openssl/ -lcrypto -lssl -c -o sbk.o sbk.c
gcc -std=c99 -I/usr/local/openssl/include/openssl/ -c -o pacc.o pacc.c
gcc -std=c99 -I./randlib/include/ -I./randlib/extras -c -o main.o main.c
gcc -L./randlib/src main.o pacc.o sbk.o ./randlib/extras/entropy.o -lpcg_random -lcrypto -lssl -Wall -o main

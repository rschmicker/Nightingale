make clean;
rm night_evp_test;
rm Makefile;
./config;
make update;
make -j 12;
gcc -I./include -L. -Wall -o night_evp_test night_evp_test.c -lcrypto;
LD_LIBRARY_PATH=. ./night_evp_test;
LD_LIBRARY_PATH=. ./apps/openssl speed -elapsed -evp nightgale

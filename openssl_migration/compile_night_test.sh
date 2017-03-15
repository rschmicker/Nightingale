make clean;
rm night_test;
rm Makefile;
./config;
make update;
make -j 12;
gcc -I./include -L. -Wextra -Wall -o night_test night_test.c -lcrypto;
LD_LIBRARY_PATH=. ./night_test;

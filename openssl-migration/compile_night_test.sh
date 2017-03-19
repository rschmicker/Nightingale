rm -f night_test
rm -f night_evp_test
./config && make update && make -j 12 && \
gcc -I./include -L. -Wextra -Wall -o night_evp_test night_evp_test.c -lcrypto && \
gcc -I./include -L. -Wextra -Wall -o night_test night_test.c -lcrypto && \
LD_LIBRARY_PATH=. ./night_test && \
LD_LIBRARY_PATH=. ./night_evp_test && \
LD_LIBRARY_PATH=. ./apps/openssl speed -elapsed -evp nightgale

PATCH=nightgale-ssl-0.9.patch

wget https://github.com/openssl/openssl/archive/master.zip && \
unzip master.zip && \
cp $PATCH openssl-master/ && \
cd openssl-master/ && \
patch -p1 < $PATCH && \
./config && \
make update && \
make -j 12 && \
gcc -I./include -L. -Wextra -Wall -o night_evp_test night_evp_test.c -lcrypto && \
gcc -I./include -L. -Wextra -Wall -o night_test night_test.c -lcrypto && \
LD_LIBRARY_PATH=. ./night_test && \
LD_LIBRARY_PATH=. ./night_evp_test && \
LD_LIBRARY_PATH=. ./apps/openssl speed -elapsed -evp nightgale

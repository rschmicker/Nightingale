PATCH=nightgale-ssl-0.9.patch

wget "https://www.openssl.org/source/openssl-1.1.0f.tar.gz"
wget "https://www.openssl.org/source/openssl-1.1.0f.tar.gz.sha256"

if [[sha256sum openssl-1.1.0f.tar.gz -ne $(cat openssl-1.1.0f.tar.gz.sha256)]];
then
    exit 1
fi
tar -xvf openssl-1.1.0f.tar.gz
mv openssl-1.1.0f/ openssl/
cp $PATCH openssl/ && \
cd openssl/ && \
patch -p1 < $PATCH && \
./config && \
make update && \
make -j $(nproc --all) && \
gcc -I./include -L. -Wextra -Wall -o night_evp_test night_evp_test.c -lcrypto && \
gcc -I./include -L. -Wextra -Wall -o night_test night_test.c -lcrypto && \
LD_LIBRARY_PATH=. ./night_test && \
LD_LIBRARY_PATH=. ./night_evp_test && \
LD_LIBRARY_PATH=. ./apps/openssl speed -elapsed -evp nightgale

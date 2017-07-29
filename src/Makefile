#DEBUG=-g -O0
#DEBUG=-O2
DEBUG=-O3
#DEBUG=-O1

#CC=gcc-6
#CC=gcc
CC=icc
#CC=/usr/local/Cellar/gcc/7.1.0/bin/gcc-7

COPTIONS=-std=c11 -Wall -Wextra -march=core-avx2 -ipo -axCORE-AVX2 -parallel -I/usr/local/include/openssl/

LDOPTIONS=-lcrypto -pthread

CMD=nightgale

ED_FILES=private.pem

OBJECTS=sub_t.o nightgale.o nightgale_p.o nightgale_pv.o test.o mysecond.o

.c.o:
	$(CC) $(COPTIONS) $(DEBUG) -c $<

$(CMD): $(OBJECTS)
	$(CC) -o $(CMD) $(OBJECTS) $(LIBS) $(LDOPTIONS)

clean:
	rm -f $(OBJECTS) $(CMD) $(ED_FILES) $(UT_OBJECTS) $(TEST)

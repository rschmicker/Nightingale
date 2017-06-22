#DEBUG=-g -O0
#DEBUG=-O2
DEBUG=-O3
#DEBUG=-O1

#CC=gcc-6
CC=gcc

COPTIONS=-std=c11 -Wall -Wextra -I/usr/local/include/openssl/

LDOPTIONS=-lcrypto -pthread

CMD=nightgale

ED_FILES=private.pem

OBJECTS=sub_t.o nightgale.o nightgale_p.o test.o mysecond.o

.c.o:
	$(CC) $(COPTIONS) $(DEBUG) -c $<

$(CMD): $(OBJECTS)
	$(CC) -o $(CMD) $(OBJECTS) $(LIBS) $(LDOPTIONS)

clean:
	rm -f $(OBJECTS) $(CMD) $(ED_FILES) $(UT_OBJECTS) $(TEST)

COPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
SBKOPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
LDOPTIONS=-L./randlib/src -lpcg_random -lcrypto -lssl -Wall
EXTRAOBJS=./randlib/extras/entropy.o

CMD=testprog

OBJECTS=sbk.o pacc.o main.o

.c.o:
	gcc $(COPTIONS) -c $<

$(CMD): $(OBJECTS)
	gcc -o testprog $(OBJECTS) $(EXTRAOBJS) $(LDOPTIONS)

sbk.o:
	gcc $(SBKOPTIONS) -c $*.c

clean:
	rm -f $(OBJECTS) testprog

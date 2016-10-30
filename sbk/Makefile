COPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
SBKOPTIONS=$(COPTIONS)
LDOPTIONS=-L./randlib/src -lpcg_random -lcrypto -lssl -Wall
LIBS=
#LIBS=randlib/src/libpcg_random.a

CMD=testprog

OBJECTS=sbk.o pacc.o main.o

.c.o:
	gcc $(COPTIONS) -c $<

$(CMD): $(OBJECTS)
	gcc -o testprog $(OBJECTS) $(LIBS) $(LDOPTIONS)

sbk.o:
	gcc $(SBKOPTIONS) -c $*.c

clean:
	rm -f $(OBJECTS) testprog

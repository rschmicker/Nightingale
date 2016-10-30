DEBUG=-g -O0

COPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
COPTIONS+=$(DEBUG)
SBKOPTIONS=$(COPTIONS)
LDOPTIONS=-L./randlib/src -lpcg_random -lcrypto -lssl -Wall
LIBS=
LDOPTIONS+=$(DEBUG)

CMD=testprog

OBJECTS=sbk.o pacc.o main.o

.c.o:
	gcc $(COPTIONS) $(DEBUG) -c $<

$(CMD): $(OBJECTS)
	gcc -o testprog $(OBJECTS) $(LIBS) $(LDOPTIONS)

#This is here as an example - sbk.o does not appear to require special options
sbk.o:
	gcc $(SBKOPTIONS) -c $*.c

clean:
	rm -f $(OBJECTS) testprog

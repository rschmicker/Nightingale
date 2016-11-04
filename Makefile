.PHONY: clean distclean
DEBUG=-g -O0
#DEBUG=-O3

COPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
LDOPTIONS=-L./randlib/src -lpcg_random -lcrypto -lssl -Wall
LIBS=
LDOPTIONS+=$(DEBUG)
RANDLIB=randlib/src/libpcg_random.so

CMD=main

ED_FILES=decrypted_file.txt encrypted_file.txt night.key

OBJECTS=sub_t.o nightgale_c.o main.o stringinfo.o

.c.o:
	gcc $(COPTIONS) $(DEBUG) -c $<

$(CMD): $(OBJECTS) $(RANDLIB)
	gcc -o $(CMD) $(OBJECTS) $(LIBS) $(LDOPTIONS)

$(RANDLIB):
	echo "Remaking randlib..."
	cd randlib; make

clean:
	rm -f $(OBJECTS) $(CMD) $(ED_FILES)

distclean: clean
	cd randlib; $(MAKE) clean

install:
	cd randlib; sudo $(MAKE) install; sudo ldconfig -v

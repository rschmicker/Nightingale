.PHONY: clean distclean
#DEBUG=-g -O0
DEBUG=-O3

COPTIONS=-std=c99 -I./randlib/include/ -I./randlib/extras/ -I/usr/local/openssl/include/openssl/
COPTIONS_UT=-std=c99 -I/usr/local/openssl/include/openssl/ -Dpcg64_random_t=char -Dpcg128_t=char -DUNIT_TEST=1

LDOPTIONS=-L./randlib/src -lpcg_random -lcrypto -lssl -Wall
LDOPTIONS_UT=-lcrypto -lssl -Wall $(DEBUG) 

LIBS=
LDOPTIONS+=$(DEBUG)
RANDLIB=randlib/src/libpcg_random.so
#RANDLIB=randlib/src/libpcg_random.a

# Give this some color!
RED=\033[0;31m
NC=\033[0m
GREEN=\033[0;32m

CMD=main

TEST=test

ED_FILES=decrypted_file.txt encrypted_file.txt night.key

OBJECTS=sub_t.o nightgale_c.o main.o mysecond.o

UT_OBJECTS=unit_test_rand.o sub_t.o nightgale_c.o unit_test.o mysecond.o

.c.o:
	gcc $(COPTIONS) $(DEBUG) -c $<
	

$(CMD): $(OBJECTS) $(RANDLIB)
	gcc -o $(CMD) $(OBJECTS) $(LIBS) $(LDOPTIONS)

$(TEST): $(UT_OBJECTS) 
	gcc -o $(TEST) $(UT_OBJECTS) $(LIBS) $(LDOPTIONS_UT)

$(RANDLIB):
	echo "Remaking randlib..."
	cd randlib; make

clean:
	rm -f $(OBJECTS) $(CMD) $(ED_FILES)

distclean: clean
	cd randlib; $(MAKE) clean

install:
	cd randlib; sudo $(MAKE) install; sudo ldconfig -v

unit_test: $(UT_OBJECTS)
	gcc -o $(TEST) $(UT_OBJECTS) $(LIBS) $(LDOPTIONS_UT)
	@./$(TEST) || (echo "${RED}Unit test failed $$?${NC}"; exit 1)
	@echo "${GREEN}Unit test passed!${NC}"

unit_test_clean:
	rm -f $(UT_OBJECTS) $(TEST) $(ED_FILES)


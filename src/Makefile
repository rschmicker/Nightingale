#.PHONY: clean distclean
#DEBUG=-g -O0
WDEBUG=-O3

COPTIONS=-std=c99 -I/usr/local/openssl/include/openssl/ -Wall -Wextra -O3
COPTIONS_UT=-std=c99 -I/usr/local/openssl/include/openssl/ -Dpcg64_random_t=char -Dpcg128_t=char -DUNIT_TEST=1

LDOPTIONS=-lcrypto
#LDOPTIONS_UT=-lcrypto -lssl -Wall $(DEBUG)

#RANDLIB=randlib/src/libpcg_random.so
#RANDLIB=randlib/src/libpcg_random.a

# Give this some color!
RED=\033[0;31m
NC=\033[0m
GREEN=\033[0;32m

CMD=nightgale

TEST=unit_test

ED_FILES=private.pem

OBJECTS=sub_t.o nightgale_c.o test.o mysecond.o

UT_OBJECTS=unit_test_rand.o sub_t.o nightgale_c.o unit_test.o mysecond.o

.c.o:
	gcc $(COPTIONS) $(DEBUG) -c $<


$(CMD): $(OBJECTS) 
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
	$(MAKE) unit_test_clean

unit_test_clean:
	rm -f $(UT_OBJECTS) $(TEST) $(ED_FILES)

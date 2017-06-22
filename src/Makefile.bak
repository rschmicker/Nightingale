#.PHONY: clean distclean
#DEBUG=-g -O0
#DEBUG=-O2
DEBUG=-O3
#DEBUG=-O1

#CC=clang
CC=gcc

#COPTIONS=-std=c11 -I/usr/local/openssl/include/openssl/ -Wall -Wextra
COPTIONS=-std=c11 -Wall -Wextra
COPTIONS_UT=-std=c99 -I/usr/local/openssl/include/openssl/ -Dpcg64_random_t=char -Dpcg128_t=char -DUNIT_TEST=1
COPTIONS_UT=-std=c99 -I/usr/local/openssl/include/openssl/ -Dpcg64_random_t=char -Dpcg128_t=char -DUNIT_TEST=1

LDOPTIONS=-lcrypto -pthread

# Give this some color!
RED=\033[0;31m
NC=\033[0m
GREEN=\033[0;32m

CMD=nightgale

TEST=unit_test

ED_FILES=private.pem

OBJECTS=sub_t.o nightgale.o nightgale_p.o test.o mysecond.o

#UT_OBJECTS=unit_test_rand.o unit_test.o $(OBJECTS)

.c.o:
	$(CC) $(COPTIONS) $(DEBUG) -c $<

$(CMD): $(OBJECTS) 
	$(CC) -o $(CMD) $(OBJECTS) $(LIBS) $(LDOPTIONS)

#$(TEST): $(UT_OBJECTS)
	#$(CC) -o $(TEST) $(UT_OBJECTS) $(LIBS) $(LDOPTIONS_UT)

clean:
	rm -f $(OBJECTS) $(CMD) $(ED_FILES) $(UT_OBJECTS) $(TEST)

#unit_test: $(UT_OBJECTS)
#$(CC) -o $(TEST) $(UT_OBJECTS) $(LIBS) $(LDOPTIONS_UT)
#@./$(TEST) || (echo "${RED}Unit test failed $$?${NC}"; exit 1)
#@echo "${GREEN}Unit test passed!${NC}"
#$(MAKE) unit_test_clean

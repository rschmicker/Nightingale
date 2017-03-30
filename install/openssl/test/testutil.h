/*
 * Copyright 2014-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_TESTUTIL_H
# define HEADER_TESTUTIL_H

#include <stdarg.h>

#include <openssl/err.h>
#include <openssl/e_os2.h>

/*-
 * Simple unit tests should implement register_tests() from test_main.h
 * and link against test_main.c.
 * To register tests, call ADD_TEST or ADD_ALL_TESTS:
 *
 * #include "test_main.h"
 *
 * void register_tests(void)
 * {
 *     ADD_TEST(test_foo);
 *     ADD_ALL_TESTS(test_bar, num_test_bar);
 * }
 *
 * Tests that need to perform custom setup or read command-line arguments should
 * implement test_main() from test_main_custom.h and link against
 * test_main_custom.c:
 *
 * int test_main(int argc, char *argv[])
 * {
 *     int ret;
 *
 *     // Custom setup ...
 *
 *     ADD_TEST(test_foo);
 *     ADD_ALL_TESTS(test_bar, num_test_bar);
 *     // Add more tests ...
 *
 *     ret = run_tests(argv[0]);
 *
 *     // Custom teardown ...
 *
 *     return ret;
 * }
 */

/* Adds a simple test case. */
# define ADD_TEST(test_function) add_test(#test_function, test_function)

/*
 * Simple parameterized tests. Calls test_function(idx) for each 0 <= idx < num.
 */
# define ADD_ALL_TESTS(test_function, num) \
  add_all_tests(#test_function, test_function, num)

/*-
 * Test cases that share common setup should use the helper
 * SETUP_TEST_FIXTURE and EXECUTE_TEST macros for test case functions.
 *
 * SETUP_TEST_FIXTURE will call set_up() to create a new TEST_FIXTURE_TYPE
 * object called "fixture". It will also allocate the "result" variable used
 * by EXECUTE_TEST. set_up() should take a const char* specifying the test
 * case name and return a TEST_FIXTURE_TYPE by value.
 *
 * EXECUTE_TEST will pass fixture to execute_func() by value, call
 * tear_down(), and return the result of execute_func(). execute_func() should
 * take a TEST_FIXTURE_TYPE by value and return 1 on success and 0 on
 * failure.
 *
 * Unit tests can define their own SETUP_TEST_FIXTURE and EXECUTE_TEST
 * variations like so:
 *
 * #define SETUP_FOOBAR_TEST_FIXTURE()\
 *   SETUP_TEST_FIXTURE(FOOBAR_TEST_FIXTURE, set_up_foobar)
 *
 * #define EXECUTE_FOOBAR_TEST()\
 *   EXECUTE_TEST(execute_foobar, tear_down_foobar)
 *
 * Then test case functions can take the form:
 *
 * static int test_foobar_feature()
 *      {
 *      SETUP_FOOBAR_TEST_FIXTURE();
 *      [...set individual members of fixture...]
 *      EXECUTE_FOOBAR_TEST();
 *      }
 */
# define SETUP_TEST_FIXTURE(TEST_FIXTURE_TYPE, set_up)\
    TEST_FIXTURE_TYPE fixture = set_up(TEST_CASE_NAME); \
    int result = 0

# define EXECUTE_TEST(execute_func, tear_down)\
        result = execute_func(fixture);\
        tear_down(fixture);\
        return result

/* Shorthand if tear_down does nothing. */
# define EXECUTE_TEST_NO_TEARDOWN(execute_func)\
        result = execute_func(fixture);\
        return result

/*
 * TEST_CASE_NAME is defined as the name of the test case function where
 * possible; otherwise we get by with the file name and line number.
 */
# if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#  if defined(_MSC_VER)
#   define TEST_CASE_NAME __FUNCTION__
#  else
#   define testutil_stringify_helper(s) #s
#   define testutil_stringify(s) testutil_stringify_helper(s)
#   define TEST_CASE_NAME __FILE__ ":" testutil_stringify(__LINE__)
#  endif                        /* _MSC_VER */
# else
#  define TEST_CASE_NAME __func__
# endif                         /* __STDC_VERSION__ */

/*
 * Internal helpers. Test programs shouldn't use these directly, but should
 * rather link to one of the helper main() methods.
 */

/* setup_test() should be called as the first thing in a test main(). */
void setup_test(void);
/*
 * finish_test() should be called as the last thing in a test main().
 * The result of run_tests() should be the input to finish_test().
 */
__owur int finish_test(int ret);

void add_test(const char *test_case_name, int (*test_fn) ());
void add_all_tests(const char *test_case_name, int (*test_fn)(int idx), int num);
__owur int run_tests(const char *test_prog_name);

/*
 *  Test assumption verification helpers.
 */

# if defined(__GNUC__)
#define PRINTF_FORMAT(a, b)   __attribute__ ((format(printf, a, b)))
# else
#define PRINTF_FORMAT(a, b)
#endif

#  define DECLARE_COMPARISON(type, name, opname)                        \
    int test_ ## name ## _ ## opname(const char *, int,                 \
                                     const char *, const char *,        \
                                     const type, const type);

# define DECLARE_COMPARISONS(type, name)                                \
    DECLARE_COMPARISON(type, name, eq)                                  \
    DECLARE_COMPARISON(type, name, ne)                                  \
    DECLARE_COMPARISON(type, name, lt)                                  \
    DECLARE_COMPARISON(type, name, le)                                  \
    DECLARE_COMPARISON(type, name, gt)                                  \
    DECLARE_COMPARISON(type, name, ge)

DECLARE_COMPARISONS(int, int)
DECLARE_COMPARISONS(unsigned int, uint)
DECLARE_COMPARISONS(char, char)
DECLARE_COMPARISONS(unsigned char, uchar)
DECLARE_COMPARISONS(long, long)
DECLARE_COMPARISONS(unsigned long, ulong)
DECLARE_COMPARISONS(size_t, size_t)

/*
 * Pointer comparisons against other pointers and null.
 * These functions return 1 if the test is true.
 * Otherwise, they return 0 and pretty-print diagnostics.
 * These should not be called directly, use the TEST_xxx macros below instead.
 */
DECLARE_COMPARISON(void *, ptr, eq)
DECLARE_COMPARISON(void *, ptr, ne)
int test_ptr(const char *file, int line, const char *s, const void *p);
int test_ptr_null(const char *file, int line, const char *s, const void *p);

/*
 * Equality tests for strings where NULL is a legitimate value.
 * These calls return 1 if the two passed strings compare true.
 * Otherwise, they return 0 and pretty-print diagnostics.
 * These should not be called directly, use the TEST_xxx macros below instead.
 */
DECLARE_COMPARISON(char *, str, eq)
DECLARE_COMPARISON(char *, str, ne)

/*
 * Equality test for memory blocks where NULL is a legitimate value.
* These calls return 1 if the two memory blocks compare true.
 * Otherwise, they return 0 and pretty-print diagnostics.
 * These should not be called directly, use the TEST_xxx macros below instead.
 */
int test_mem_eq(const char *, int, const char *, const char *,
                const void *, size_t, const void *, size_t);
int test_mem_ne(const char *, int, const char *, const char *,
                const void *, size_t, const void *, size_t);

/*
 * Check a boolean result for being true or false.
 * They return 1 if the condition is true (i.e. the value is non-zro).
 * Otherwise, they return 0 and pretty-prints diagnostics using |desc|.
 * These should not be called directly, use the TEST_xxx macros below instead.
 */
int test_true(const char *file, int line, const char *s, int b);
int test_false(const char *file, int line, const char *s, int b);

/*
 * Pretty print a failure message.
 * These should not be called directly, use the TEST_xxx macros below instead.
 */
void test_error(const char *file, int line, const char *desc, ...)
    PRINTF_FORMAT(3, 4);
void test_error_c90(const char *desc, ...) PRINTF_FORMAT(1, 2);
void test_info(const char *file, int line, const char *desc, ...)
    PRINTF_FORMAT(3, 4);
void test_info_c90(const char *desc, ...) PRINTF_FORMAT(1, 2);

/*
 * The following macros provide wrapper calls to the test functions with
 * a default description that indicates the file and line number of the error.
 */
# define TEST_int_eq(a, b)    test_int_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ne(a, b)    test_int_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_lt(a, b)    test_int_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_le(a, b)    test_int_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_gt(a, b)    test_int_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ge(a, b)    test_int_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_int_eq(a, b)    test_int_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ne(a, b)    test_int_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_lt(a, b)    test_int_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_le(a, b)    test_int_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_gt(a, b)    test_int_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_int_ge(a, b)    test_int_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_uint_eq(a, b)    test_uint_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uint_ne(a, b)    test_uint_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uint_lt(a, b)    test_uint_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uint_le(a, b)    test_uint_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uint_gt(a, b)    test_uint_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uint_ge(a, b)    test_uint_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_char_eq(a, b)    test_char_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_char_ne(a, b)    test_char_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_char_lt(a, b)    test_char_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_char_le(a, b)    test_char_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_char_gt(a, b)    test_char_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_char_ge(a, b)    test_char_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_uchar_eq(a, b)    test_uchar_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uchar_ne(a, b)    test_uchar_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uchar_lt(a, b)    test_uchar_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uchar_le(a, b)    test_uchar_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uchar_gt(a, b)    test_uchar_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_uchar_ge(a, b)    test_uchar_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_long_eq(a, b)    test_long_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_long_ne(a, b)    test_long_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_long_lt(a, b)    test_long_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_long_le(a, b)    test_long_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_long_gt(a, b)    test_long_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_long_ge(a, b)    test_long_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_ulong_eq(a, b)    test_ulong_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ulong_ne(a, b)    test_ulong_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ulong_lt(a, b)    test_ulong_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ulong_le(a, b)    test_ulong_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ulong_gt(a, b)    test_ulong_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ulong_ge(a, b)    test_ulong_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_size_t_eq(a, b)    test_size_t_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_ne(a, b)    test_size_t_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_lt(a, b)    test_size_t_lt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_le(a, b)    test_size_t_le(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_gt(a, b)    test_size_t_gt(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_size_t_ge(a, b)    test_size_t_ge(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_ptr_eq(a, b)    test_ptr_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ptr_ne(a, b)    test_ptr_ne(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_ptr(a)          test_ptr(__FILE__, __LINE__, #a, a)
# define TEST_ptr_null(a)     test_ptr_null(__FILE__, __LINE__, #a, a)

# define TEST_str_eq(a, b)    test_str_eq(__FILE__, __LINE__, #a, #b, a, b)
# define TEST_str_ne(a, b)    test_str_ne(__FILE__, __LINE__, #a, #b, a, b)

# define TEST_mem_eq(a, m, b, n) test_mem_eq(__FILE__, __LINE__, #a, #b, a, m, b, n)
# define TEST_mem_ne(a, m, b, n) test_mem_ne(__FILE__, __LINE__, #a, #b, a, m, b, n)

# define TEST_true(a)         test_true(__FILE__, __LINE__, #a, a)
# define TEST_false(a)        test_false(__FILE__, __LINE__, #a, a)

/*
 * TEST_error(desc, ...) prints an informative error message in the standard
 * format.  |desc| is a printf format string.
 */
# if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#  define TEST_error         test_error_c90
#  define TEST_info          test_info_c90
# else
#  define TEST_error(...)    test_error(__FILE__, __LINE__, __VA_ARGS__)
#  define TEST_info(...)     test_info(__FILE__, __LINE__, __VA_ARGS__)
# endif

/*
 * For "impossible" conditions such as malloc failures or bugs in test code,
 * where continuing the test would be meaningless. Note that OPENSSL_assert
 * is fatal, and is never compiled out.
 */
# define TEST_check(condition)                  \
    do {                                        \
        if (!(condition)) {                     \
            ERR_print_errors_fp(stderr);        \
            OPENSSL_assert(!#condition);        \
        }                                       \
    } while (0)
#endif                          /* HEADER_TESTUTIL_H */

/*-------------------------------------------------------------------------
 *
 * stringinfo.c
 *
 * StringInfo provides an indefinitely-extensible string data type.
 * It can be used to buffer either ordinary C strings (null-terminated text)
 * or arbitrary binary data.  All storage is allocated with malloc().
 *
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *    src/backend/lib/stringinfo.c
 *
 *-------------------------------------------------------------------------
 */

#include "stringinfo.h"

#define Size long long int
#define MaxAllocSize (2^30)

/*
 * makeStringInfo
 *
 * Create an empty 'StringInfoData' & return a pointer to it.
 */
StringInfo
makeStringInfo(void)
{
    StringInfo  res;

    res = (StringInfo) malloc(sizeof(StringInfoData));

    initStringInfo(res);

    return res;
}

/*
 * initStringInfo
 *
 * Initialize a StringInfoData struct (with previously undefined contents)
 * to describe an empty string.
 */
void
initStringInfo(StringInfo str)
{
    int         size = 1024;    /* initial default buffer size */

    str->data = (char *) malloc(size);
    str->maxlen = size;
    resetStringInfo(str);
}

/*
 * resetStringInfo
 *
 * Reset the StringInfo: the data buffer remains valid, but its
 * previous content, if any, is cleared.
 */
void
resetStringInfo(StringInfo str)
{
    str->data[0] = '\0';
    str->len = 0;
    str->cursor = 0;
}

/*
 * appendStringInfoString
 *
 * Append a null-terminated string to str.
 * Like appendStringInfo(str, "%s", s) but faster.
 */
void
appendStringInfoString(StringInfo str, const char *s)
{
    appendBinaryStringInfo(str, s, strlen(s));
}

/*
 * appendStringInfoChar
 *
 * Append a single byte to str.
 * Like appendStringInfo(str, "%c", ch) but much faster.
 */
void
appendStringInfoChar(StringInfo str, char ch)
{
    /* Make more room if needed */
    if (str->len + 1 >= str->maxlen)
        enlargeStringInfo(str, 1);

    /* OK, append the character */
    str->data[str->len] = ch;
    str->len++;
    str->data[str->len] = '\0';
}

/*
 * appendStringInfoSpaces
 *
 * Append the specified number of spaces to a buffer.
 */
void
appendStringInfoSpaces(StringInfo str, int count)
{
    if (count > 0)
    {
        /* Make more room if needed */
        enlargeStringInfo(str, count);

        /* OK, append the spaces */
        while (--count >= 0)
            str->data[str->len++] = ' ';
        str->data[str->len] = '\0';
    }
}

/*
 * appendBinaryStringInfo
 *
 * Append arbitrary binary data to a StringInfo, allocating more space
 * if necessary.
 */
void
appendBinaryStringInfo(StringInfo str, const char *data, int datalen)
{

    /* Make more room if needed */
    enlargeStringInfo(str, datalen);

    /* OK, append the data */
    memcpy(str->data + str->len, data, datalen);
    str->len += datalen;

    /*
     * Keep a trailing null in place, even though it's probably useless for
     * binary data.  (Some callers are dealing with text but call this because
     * their input isn't null-terminated.)
     */
    str->data[str->len] = '\0';
}

/*
 * enlargeStringInfo
 *
 * Make sure there is enough space for 'needed' more bytes
 * ('needed' does not include the terminating null).
 *
 * External callers usually need not concern themselves with this, since
 * all stringinfo.c routines do it automatically.  However, if a caller
 * knows that a StringInfo will eventually become X bytes large, it
 * can save some malloc overhead by enlarging the buffer before starting
 * to store data in it.
 *
 * NB: because we use realloc() to enlarge the buffer, the string buffer
 * will remain allocated in the same memory context that was current when
 * initStringInfo was called, even if another context is now current.
 * This is the desired and indeed critical behavior!
 */
void
enlargeStringInfo(StringInfo str, int needed)
{
    int         newlen;

    /*
     * Guard against out-of-range "needed" values.  Without this, we can get
     * an overflow or infinite loop in the following.
     */
    if (needed < 0) {             /* should not happen */
	printf("Error: invalid string enlargement request size: %d\n", needed);
	exit(1);
    }
        //elog(ERROR, "invalid string enlargement request size: %d", needed);
    if (((Size) needed) >= (MaxAllocSize - (Size) str->len)) {
	printf("Error: out of memory, cannot enlarge string buffer containing %d bytes by %d more bytes.\n", str->len, needed);
	exit(1);
    }
/*
        ereport(ERROR,
                (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                 errmsg("out of memory"),
                 errdetail("Cannot enlarge string buffer containing %d bytes by %d more bytes.",
                           str->len, needed)));
*/

    needed += str->len + 1;     /* total space required now */

    /* Because of the above test, we now have needed <= MaxAllocSize */

    if (needed <= str->maxlen)
        return;                 /* got enough space already */

    /*
     * We don't want to allocate just a little more space with each append;
     * for efficiency, double the buffer size each time it overflows.
     * Actually, we might need to more than double it if 'needed' is big...
     */
    newlen = 2 * str->maxlen;
    while (needed > newlen)
        newlen = 2 * newlen;

    /*
     * Clamp to MaxAllocSize in case we went past it.  Note we are assuming
     * here that MaxAllocSize <= INT_MAX/2, else the above loop could
     * overflow.  We will still have newlen >= needed.
     */
    if (newlen > (int) MaxAllocSize)
        newlen = (int) MaxAllocSize;

    str->data = (char *) realloc(str->data, newlen);
    str->maxlen = newlen;
}


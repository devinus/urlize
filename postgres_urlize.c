/*
 * urlize - search engine optimized URLs
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */
#include <postgres.h>
#include <fmgr.h>
#include <stdio.h>

#ifdef PG_MODULE_MAGIC
    PG_MODULE_MAGIC;
#endif

Datum urlize(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(urlize);

/**
 * Returns a pretty, hyphenated string for search engine optimized URLs.
 *
 * To use within PostgreSQL as a function:
 *
 *     CREATE OR REPLACE FUNCTION
 *         hello(varchar)
 *     RETURNS
 *         varchar
 *     AS
 *        'urlize.so', 'urlize'
 *     LANGUAGE
 *         C
 *     STRICT
 *     IMMUTABLE;
 *
 * Example:
 * template1=# SELECT urlize('Ruby vs. Python: The Benefits of Monkeypatching
 * and Chainability');
 * ruby-vs-python-the-benefits-of-monkeypatching-and-chainability
 */
Datum urlize(PG_FUNCTION_ARGS)
{
    VarChar *src = PG_GETARG_VARCHAR_P(0);
    VarChar *res;
    char *buf1;
    char *buf2;
    char c;
    int len;
    unsigned int i, j;
    unsigned short int repeat = 0;

    len = strlen(VARDATA(src));
    buf1 = (char *) palloc((len + 1) * sizeof(char));
    if (buf1 == NULL) {
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory")));
        PG_RETURN_NULL();
    }

    buf2 = (char *) palloc((len + 1) * sizeof(char));
    if (buf2 == NULL) {
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory")));
        PG_RETURN_NULL();
    }

    memcpy(buf1, VARDATA(src), len);

    /* Replace all non-alphanumeric characters with a dash */
    for (i = 0, j = 0; i < len; ++i) {
        c = buf1[i];
        if (c == '\'')
            continue;
        else if (isalnum(c))
            buf2[j] = tolower(c);
        else
            buf2[j] = '-';
        ++j;
    }
    buf2[j] = '\0';

    /* Reduce repeating dashes */
    for (i = 0, j = 0; i < len; ++i) {
        c = buf2[i];
        if (c == '-') {
            if (repeat)
                continue;
            repeat = 1;
        } else {
            repeat = 0;
        }
        buf1[j] = c;
        ++j;
    }
    buf1[j] = '\0';

    /* Strip leading dashes */
    len = strlen(buf1);

    i = 0;
    while (i < len) {
        if (buf1[i] != '-')
            break;
        ++i;
    }
    strcpy(buf2, buf1 + i);

    /* Strip trailing dashes */
    len = strlen(buf2);

    i = len - 1;
    while (i >= 0) {
        if (buf2[i] != '-')
            break;
        --i;
    }
    strncpy(buf1, buf2, i + 1);
    buf1[i+1] = '\0';

    pfree(buf2);

    res = (VarChar *) palloc(VARSIZE(src));
    SET_VARSIZE(res, VARSIZE(src));
    memcpy((void *) VARDATA(res), (void *) buf1, i + 2);

    pfree(buf1);

    PG_RETURN_VARCHAR_P(res);
}

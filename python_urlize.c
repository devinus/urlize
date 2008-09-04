#include <Python.h>

/*
 * Returns a pretty, hyphenated string for search engine optimized URLs.
 *
 * To use within PostgreSQL as a PL/Python function:
 *
 *    CREATE OR REPLACE FUNCTION urlize (s varchar)
 *        RETURNS VARCHAR
 *    AS $$
 *        from perxcore.lib.urlize import python_urlize
 *        return python_urlize(s)
 *    $$ LANGUAGE plpythonu;
 *
 * Example:
 * >>> urlize('Ruby vs. Python: The Benefits of Monkeypatching and Chainability')
 * ruby-vs-python-the-benefits-of-monkeypatching-and-chainability
 */
static PyObject* urlize(PyObject *self, PyObject *args)
{
    PyObject* res;
    const char* src;
    char* buf1;
    char* buf2;
    char c;
    size_t len;
    unsigned int i, j;
    unsigned short int repeat = 0;

    if (!PyArg_ParseTuple(args, "z", &src))
        return NULL;

    if (src == NULL)
        return PyString_FromString("");

    len = strlen(src);
    buf1 = (char*) PyMem_New(char, len + 1);
    if (buf1 == NULL)
        return PyErr_NoMemory();

    /* Replace all non-alphanumeric characters with a dash */
    for (i = 0; i < len; ++i) {
        c = src[i];
        if (isalnum(c))
            buf1[i] = tolower(c);
        else
            buf1[i] = '-';
    }
    buf1[i] = '\0';

    len = strlen(buf1);
    buf2 = (char*) PyMem_New(char, len + 1);
    if (buf2 == NULL)
        return PyErr_NoMemory();

    /* Reduce multiple dashes to only one */
    for (i = 0, j = 0; i < len; ++i) {
        c = buf1[i];
        if (c == '-') {
            if (repeat)
                continue;
            repeat = 1;
        } else {
            repeat = 0;
        }
        buf2[j] = c;
        ++j;
    }
    buf2[j] = '\0';

    /* Strip leading dashes */
    len = strlen(buf2);

    i = 0;
    while (i < len) {
        if (buf2[i] != '-')
            break;
        ++i;
    }
    strcpy(buf1, buf2+i);

    /* Strip trailing dashes */
    len = strlen(buf1);

    i = len - 1;
    while (i >= 0) {
        if (buf1[i] != '-')
            break;
        --i;
    }
    strncpy(buf2, buf1, i+1);
    buf2[i+1] = '\0';

    PyMem_Del(buf1);

    res = PyString_FromString(buf2);
    PyMem_Del(buf2);
    return res;
}

static PyMethodDef methods[] = {
    {"urlize", urlize, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initurlize(void)
{
    (void) Py_InitModule("urlize", methods);
}

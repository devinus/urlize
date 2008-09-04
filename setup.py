from distutils.core import setup, Extension

urlize = Extension('urlize', sources=['python_urlize.c'])

setup(
    name='urlize',
    version='0.1',
    ext_modules=[urlize]
)

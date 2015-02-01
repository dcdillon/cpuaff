#!/bin/bash

## let the Makefile help us some
test -f Makefile && make distclean

## remove autotools generated files
rm -rf Makefile.in \
    aclocal.m4 \
    autom4te.cache/ \
    compile \
    config.guess \
    config.h.in \
    config.sub \
    configure \
    depcomp \
    examples/Makefile.in \
    include/Makefile.in \
    install-sh \
    missing \
    test-driver \
    tests/Makefile.in \
    examples/Makefile.in \
    include/Makefile.in

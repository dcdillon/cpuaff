#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd $DIR  > /dev/null 2>&1

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

## remove (maybe) some other generated files
rm -rf packaging/deb
rm -rf cpuaff*.tar.gz
rm -rf packaging/deb.*

popd > /dev/null 2>&1

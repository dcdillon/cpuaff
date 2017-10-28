#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd $DIR > /dev/null 2>&1

aclocal
autoconf
autoheader
automake --add-missing

popd > /dev/null 2>&1

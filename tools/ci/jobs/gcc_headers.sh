#!/usr/bin/env bash

export CC=gcc
export CXX=g++

if [ "$NEWCC" != "" ]; then
    export CC="$NEWCC"
fi
if [ "$NEWCXX" != "" ]; then
    export CXX="$NEWCXX"
fi

export LOGFILE=gcc_h.log

source ./tools/ci/scripts/init.sh

do_init

cd src
export dir=$(pwd)
export includes="-I$dir -I/usr/include -I/usr/include/libxml2"

run_compile_headers "$@"

cd ..

source ./tools/ci/scripts/exit.sh

exit 0

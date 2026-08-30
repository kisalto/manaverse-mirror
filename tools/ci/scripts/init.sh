#!/usr/bin/env bash

export dir=$(pwd)
export ERRFILE=${dir}/logs/${LOGFILE}
export ERRFILE_UNFILTERED=${ERRFILE%.log}.unfiltered.log

rm -f "$ERRFILE"
rm -f "$ERRFILE_UNFILTERED"

function do_init {
    $CC --version
    check_error $?
    $CXX --version
    check_error $?
}

retry_with_increasing_wait()
{
    local wait_time=1

    while true; do
        printf "Running %s\n" "$*"
        "$@"
        local retval=$?

        if [[ $retval -eq 0  ||  $wait_time -gt 16 ]]; then
            break
        fi

        printf "command %s failed (exit code %d), waiting %d seconds until next try.\n"\
               "$*" "$retval" "$wait_time"

        sleep "$wait_time"
        # 1, 2, 4, 8, 16...
        (( wait_time = 2 * wait_time ))
    done

    return "$retval"
}

function aptget_update {
    if [ "$RUNFROMSHELL" != "" ];
    then
        echo "Running from shell. Skipping apt-get update"
        return
    fi

    retry_with_increasing_wait apt-get update
}

function aptget_install {
    if [ "$RUNFROMSHELL" != "" ];
    then
        echo "Running from shell. Skipping apt-get"
        return
    fi

    retry_with_increasing_wait aptget_update
    retry_with_increasing_wait apt-get --assume-yes -qq --no-install-recommends install "$@"
}

function gitclone {
    retry_with_increasing_wait git clone "$1/$2" "$3"
    check_error $?
}

function check_error {
    if [ "$1" != 0 ]; then
        cp config.log logs
        cp src/test-suite.log logs
        cat $ERRFILE
        exit $1
    fi
}

function run_configure_simple {
    rm -f "$ERRFILE"
    echo "autoreconf -i"
    autoreconf -i 2>$ERRFILE
    check_error $?
    rm -f "$ERRFILE"
    echo "./configure $*"
    ./configure $* 2>$ERRFILE
    check_error $?
}

function run_configure {
    run_configure_simple $*

    rm -f "$ERRFILE"
    cd po
    echo "make update-gmo"
    make update-gmo 2>$ERRFILE
    check_error $?
    cd ..

    rm -f "$ERRFILE"
    cd po
    echo "make update-po"
    make update-po 2>$ERRFILE
    check_error $?
    cd ..
}

function run_cmake {
    rm -f "$ERRFILE"
    echo "cmake ."
    cmake . 2>$ERRFILE
    check_error $?
}

function run_make {
    rm -f "$ERRFILE"
    if [ "$JOBS" == "" ]; then
        export JOBS=2
        echo "No JOBS defined"
    fi
    echo make clean
    make clean || true
    echo "make -j${JOBS} V=0 $*"
    make -j${JOBS} V=0 $* 2>$ERRFILE
    check_error $?

    for path in "src/manaplus" "src/dyecmd"; do
        if [[ -f "$path" ]]; then
            stat -c '%s %n' "$path"
            strip -o "$path.stripped" "$path"
            stat -c '%s %n' "$path.stripped"
        else
            printf "%s does not exist, no strip & exe size summary\n" "$path"
        fi
    done
}

function run_make_check {
    rm -f "$ERRFILE"
    if [ "$JOBS" == "" ]; then
        export JOBS=2
        echo "No JOBS defined"
    fi
    echo make clean
    make clean || true
    echo "make -j${JOBS} V=0 check $*"
    make -j${JOBS} V=0 check $* 2>$ERRFILE
    export ERR=$?
    cp src/*.log logs || true
    cp src/manaplustests.trs logs || true
    cp src/Makefile logs || true
    if [ "${ERR}" != 0 ]; then
        cat $ERRFILE
        cat src/manaplustests.log
        exit ${ERR}
    fi
# disabled due performance issue. it runs bigger than gitlab limit (3h)
#    valgrind -q --read-var-info=yes --track-origins=yes --malloc-fill=11 --free-fill=55 --show-reachable=yes --leak-check=full --leak-resolution=high --partial-loads-ok=yes --error-limit=no ./src/manaplustests 2>logs/valg.log
#    export DATA=$(grep "invalid" logs/valg.log)
#    if [ "$DATA" != "" ];
#    then
#        cat logs/valg.log
#        echo "valgrind error"
#        exit 1
#    fi
# disabled due some kind of bug in valgrind. look like false positives.
#    export DATA=$(grep -A 2 "uninitialised" logs/valg.log|grep ".cpp")
#    if [ "$DATA" != "" ];
#    then
#        cat logs/valg.log
#        echo "valgrind error"
#        exit 1
#    fi
    cat logs/valg.log
    echo "valgrind check"
}

function run_gcov {
    gcovr -r . --gcov-executable=$1 --html --html-details -o coverage/$2.html
    check_error $?
    gcovr -r . --gcov-executable=$1 -o logs/$2.txt
    check_error $?
    gcovr -r . --gcov-executable=$1 --xml-pretty --exclude-unreachable-branches --print-summary -o coverage/$2.xml
    check_error $?
    cat logs/$2.txt
    check_error $?
}

function run_check_warnings {
    if [[ -s "$ERRFILE" ]]; then
        printf "Warnings detected in %s:\n" "$ERRFILE"
        cat $ERRFILE
        exit 1
    fi

    if [[ -s "$ERRFILE_UNFILTERED" ]]; then
        printf "Warnings detected in %s. The maintainer might want to take a peek, sometimes.\n" \
               "$ERRFILE_UNFILTERED"
    fi
}

function run_compile_headers {
    rm -f "$ERRFILE"
    echo "$CC -c -x c++ $* $includes */*/*/*/*.h */*/*/*.h */*/*.h */*.h *.h"
    $CC -c -x c++ $* $includes */*/*/*/*.h */*/*/*.h */*/*.h */*.h *.h 2>$ERRFILE
    if [[ -s "$ERRFILE" ]]; then
        cat $ERRFILE
        exit 1
    fi
}

function run_tarball {
    rm -f "$ERRFILE"
    echo "make dist-xz"
    make dist-xz 2>$ERRFILE
    check_error $?

    mkdir $1
    cd $1
    echo "tar xf ../*.tar.xz"
    tar xf ../*.tar.xz
    cd manaverse*
}

function run_mplint {
    rm -f "$ERRFILE"
    echo "\$ ./mplint $*"

    ./mplint "$@" > "$ERRFILE.unfiltered"

    # if grep does not find any messages, it exits non-zero
    grep -v \
         -e "src/unittests/doctest.h" \
         -e "src/unittests/catch.hpp" \
         -e "src/debug/" \
      < "$ERRFILE.unfiltered" > "$ERRFILE" || true

    run_check_warnings
}

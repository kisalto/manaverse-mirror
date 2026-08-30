#!/bin/bash

# ./build.sh            | builds the project without linting
# ./build.sh lint       | builds the project with linting (src, data, po)
# ./build.sh lintonly   | just linting (src, data, po)


##########
#Required apps check
REQUIRED_APPS='g++ gcc xsel make autoconf automake autopoint gettext libxml2-dev libcurl4-gnutls-dev libpng-dev libsdl-gfx1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-net1.2-dev libsdl-ttf2.0-dev gdb valgrind netcat-openbsd procps zlib*'
    for APP in $REQUIRED_APPS; do
        REQ_APP_CHECK=$(which "$APP")
            if [[ "$REQ_APP_CHECK" == '' ]]; then
                echo -e "$APP must be installed in order for build to run\nRequired Apps: $REQUIRED_APPS" >&2
                    read -p "Do you want to try to build anyway? (Type \"yes\" to continue)" APP_ANS
                        if [[ "$APP_ANS" == 'yes' ]]; then
                            break
                        else
                            exit 0
                        fi
            fi
    done

dir=`pwd`

if [[ ! -d "${dir}/src" ]]; then
    echo "you can only build in the root directory of the project."
    exit 1
fi

if [[ $1 == lin* ]]; then
    if [[ ! -d "logs" ]]; then
        mkdir -p logs
    fi
    echo "that can take a while, go make some coffee, tea... build a house, watch some movies... ^^'"
    ./tools/ci/jobs/mplint.sh src | tee logs/mplint_src.log
    ./tools/ci/jobs/mplint.sh po | tee logs/mplint_po.log
    ./tools/ci/jobs/mplint.sh data | tee logs/mplint_data.log
    ./tools/ci/jobs/cpplint.sh | tee logs/cpplint.log
fi
if [[ $1 != lintonly ]]; then
    autoreconf -i
    ./configure --prefix=$dir/run \
    --datadir=$dir/run/share/games \
    --bindir=$dir/run/bin \
    --mandir=$dir/run/share/man $*

    cd po
    make update-gmo
    cd ..
    make
    mkdir run
    make install
fi

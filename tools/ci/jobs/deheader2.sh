#!/usr/bin/env bash

export LOGFILE2=logs/deheader2.log

if [[ ! -s "$LOGFILE" ]]; then
    echo "Empty output. Probably wrong search flags."
    exit 1
fi

if grep "error:" "$LOGFILE"; then
    echo "Compilation error found"
    exit 1
fi

grep "deheader:" "${LOGFILE}" \
  | grep -v \
         -e "deheader: remove <climits> from ./src/being/localplayer.cpp" \
         -e "deheader: remove <fstream> from ./src/utils/files.cpp" \
         -e "deheader: remove <climits> from ./src/resources/map/map.cpp" \
         -e "deheader: remove <sys/time.h> from ./src/resources/wallpaper.cpp" \
         -e "deheader: remove <linux/tcp.h> from ./src/net/sdltcpnet.cpp" \
         -e "deheader: remove <netdb.h> from ./src/net/sdltcpnet.cpp" \
         -e "deheader: remove <netinet/in.h> from ./src/net/sdltcpnet.cpp" \
         -e "deheader: remove <climits> from ./src/resources/map/map.cpp" \
         -e "deheader: remove <vector> from ./src/utils/vector.h" \
         -e "has more than one inclusion of \"resources/dye/dyepalette_replace" \
         -e "/sdl2gfx/" \
         -e "doctest.h" \
         -e "deheader: remove <climits> from ./src/units.cpp" \
    > "${LOGFILE2}"


if [[ -s "$LOGFILE2" ]]; then
    echo "Wrong include files found:"
    cat "$LOGFILE2"
    exit 1
fi

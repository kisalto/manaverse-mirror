#!/usr/bin/env bash

set -e # Fail if any command fails
set -u # Fail if any variable is unset when used

export LOGFILE=nsiqcppstyle.log
source ./tools/ci/scripts/init.sh

rm -rf nsiqcppstyle.git
./tools/ci/scripts/retry.sh git clone https://git.themanaworld.org/mana/nsiqcppstyle nsiqcppstyle.git

# replace python -> python2 on line3. (Explicitly call python2)
sed -i -s -e '3s/^python/python2/' ./nsiqcppstyle.git/nsiqcppstyle

./nsiqcppstyle.git/nsiqcppstyle --ci --show-url src \
  | grep -v -e "catch.hpp" \
         -e "doctest.h" \
         -e "sdl2gfx" \
         -e "/debug/" \
    > "$ERRFILE_UNFILTERED"

# If grep does not find any messages, it exits non-zero. Therefore, use ||true
grep -v -E \
     -e "gamemodifiers[.]cpp(.+):  Incorrect align on long parameter list in front of '(N_|{|})'" \
     -e "graphics_calcImageRect[.]hpp(.+):  function [(]calcTileVertexesInline[)] has non named parameter" \
     -e "dye[.]cpp(.+):  Do not use goto keyword" \
     -e "item/item[.]cpp(.+):  function [(]dragDrop[)] has non named parameter" \
     -e "windowmenu[.]cpp(.+):  function [(]addButton[)] has non named parameter" \
     -e "windowmenu[.]cpp(.+):  Incorrect align on long parameter list in front of '(.|Visible_true)', it should be aligen in column 15" \
     -e "channeltab[.]h(.+):  Incorrect align on long parameter list in front of 'const'" \
     -e "/enums/simpletypes/(.+):  function [(]defIntEnum[)] has non named parameter" \
     "$ERRFILE_UNFILTERED" > "$ERRFILE" || true

run_check_warnings

echo ok

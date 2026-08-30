#!/usr/bin/env bash

set -e # Fail if any command fails
set -u # Fail if any variable is unset when used

export LOGFILE=cpplint.log
source ./tools/ci/scripts/init.sh

# Re-download if not executable or older than a day
if [[ ! -x cpplint.py \
   || $(find cpplint.py -mtime 1 -print) ]]; then
    #tools/ci/scripts/retry.sh wget "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py"
    # Google no longer offers their cpplint and suggests using the following one, instead:
    tools/ci/scripts/retry.sh wget "https://raw.githubusercontent.com/cpplint/cpplint/refs/tags/2.0.0/cpplint.py"
    chmod +x cpplint.py

    # env python -> env python3
    sed -i -s -e '1s/python$/python3/' cpplint.py
fi

declare -a args

args+=("--filter=\
-build/include,\
-whitespace/braces,-whitespace/operators,-runtime/references,\
-runtime/threadsafe_fn,-readability/streams,\
-runtime/rtti,\
-whitespace/newline,-whitespace/labels,\
-runtime/indentation_namespace,\
-whitespace/indent,\
-whitespace/parens,\
-runtime/int,\
-readability/todo,\
-runtime/arrays,\
-runtime/printf,\
-whitespace/empty_loop_body,\
-build/c++11,\
-readability/fn_size,\
-build/header_guard,\
-readability/braces,\
-legal/copyright,\
-whitespace/semicolon,\
-readability/namespace,\
-readability/casting,\
-readability/inheritance,\
-runtime/string,\
-build/c++tr1"\
)

# Cannot find a good way to handle execution errors
# (such cpplint.py demanding a version of Python that does not exist)
# So use || true here and assume any other problems will be exposed
# by the grep below. Addendum: no more grep. YMMV.
find src \
     -type f \
     \( -name "*.cc" \
     -o \( -name "*.cpp" -a \! -path src/debug/nvwa/debug_new.cpp \) \
     -o \( -name "*.h"   -a \! -path src/unittests/doctest.h \) \
     -o \( -name "*.hpp" -a \! -path src/unittests/catch.hpp \) \
     \) -exec ./cpplint.py --verbose=5 "${args[@]}" {} \+ 2>"$ERRFILE"
  #|| true



run_check_warnings

echo ok

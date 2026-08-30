#!/usr/bin/env bash
# Copyright (C) 2001-2010 Wormux Team.
# Copyright (C) 2010 The ManaWorld Development Team.
# Copyright (C) 2012-2016 The ManaPlus Developers.
# Copyright (C) 2021-2025 The ManaVerse Developers.

cd ..
if [[ $1 != "" ]]; then
  new_year="$1"
else
  new_year=$(date +'%Y')
fi

[[ ! -e src ]] && echo "This script should be ran from the top manaverse dirs ./tools/ path." && exit 2

# update the dates, creating the interval if it doesn't exist yet
# N.B: without the type f check, sed replaces symlinks with regular files
find -type f -a \
     \(     -iname "*.cpp" \
        -or -iname "*.cc" \
        -or -iname "*.h" \
        -or -iname "*.hpp" \
        -or -iname "*.in" \
        -or -iname "*.inc" \
        -or -iname "*.sh" \
        -or -iname "*.txt" \
     \) -execdir sed -i "/Copyright.*The ManaVerse Developers/ s,\(20[0-9]*\) \|\(20[0-9]*\)-20[0-9]* ,\1\2-$new_year ," {} \+

printf "The next +/- counts mentioning copyright lines should match:\n"
printf "If they do not, then try finding the offending files with grep -rl <\$bad_line>\n"
# do a semi-automated commit check
git diff | grep "^[-+][^-+]" | sort | uniq -c

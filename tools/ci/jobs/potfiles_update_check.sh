#!/usr/bin/env bash

set -e # Fail if any command fails
set -u # Fail if any variable is unset when used

export LOGFILE=potfiles.log
source ./tools/ci/scripts/init.sh

./po/POTgen.sh

# error if POTgen applied any changes
git diff >> "$ERRFILE"

# Save in-progress work in case someone runs this script manually.
#git stash push -m "POTgen run"
# Restore repository back to original state for other ci jobs.
#git reset --hard

run_check_warnings

echo ok

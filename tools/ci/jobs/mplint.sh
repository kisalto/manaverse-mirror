#!/usr/bin/env bash

set -e # Fail if any command fails
set -u # Fail if any variable is unset when used

export LOGFILE=mplint_po.log
source ./tools/ci/scripts/init.sh

rm -rf mplint || true
tools/ci/scripts/retry.sh wget -O mplint-release.zip "$MPLINT_PACKAGE_URL"
unzip mplint-release.zip

read mplint_commit < mplint-version.txt
printf "Using mplint: %s\n" "$mplint_commit"

echo " " >config.h
for task in "$@"; do
    run_mplint $task
done

source ./tools/ci/scripts/exit.sh

exit 0

#!/usr/bin/env bash

if [[ "$RUNFROMSHELL" != "" ]]; then
    if [[ ${CIRRUS_CI} != "true" ]]; then
        echo "Running from shell. Skipping run tests $*"
        exit 0
    fi
fi

export SDL_VIDEODRIVER="dummy"
ulimit -c unlimited -S
ulimit -c unlimited
rm -rf core*
sysctl -w kernel.core_pattern=core

export HOME="logs/home"
rm -rf $HOME

export ASAN_OPTIONS=detect_stack_use_after_return=true:strict_init_order=true

num_clean_runs=1
for ((run_n = 0; run_n < num_clean_runs; ++run_n)); do
    [[ $run_n -gt 0 ]] && sleep 5
    printf "Clean config test run %d\n" "$run_n"

    for f in ".config/mana/mana/config.xml" \
             ".config/mana/mana/serverlistplus.xml" \
             ".local/share/mana/manaverse.log"; do
        rm -f "$HOME/$f"
        if [[ -n $HOME && $HOME != /root* && -e /root/$f ]]; then
            printf 'Warning: \$HOME is set, but %s exists!\n' "/root/$f"
        fi
    done
    ./tools/ci/scripts/runtest.sh || exit 1
done

num_unclean_runs=0
for ((run_n = 0; run_n < num_unclean_runs; ++run_n)); do
    [[ $run_n -gt 0 ]] && sleep 5
    printf "Unclean test run %d\n" "$run_n"

    ./tools/ci/scripts/runtest.sh || exit 1
done

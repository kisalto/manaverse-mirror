#!/usr/bin/env bash

if [[ "$RUNFROMSHELL" != "" ]]; then
    echo "Running from shell. Skipping update repos"
    exit 0
fi

# dpkg fsyncs after every package, which is pointless in CI
echo force-unsafe-io > /etc/dpkg/dpkg.cfg.d/02speedup || true

if grep "nameserver 1.10.100.101" /etc/resolv.conf; then
    echo "Detected local runner"
    sed -i 's!http://httpredir.debian.org/debian!http://1.10.100.103/debian!' /etc/apt/sources.list || true
    sed -i 's!http://deb.debian.org/debian!http://1.10.100.103/debian!' /etc/apt/sources.list || true
fi

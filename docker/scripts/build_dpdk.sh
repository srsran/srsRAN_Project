#!/bin/bash
#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set -e # stop executing after error

main() {
    # Check number of args
    if [ $# -lt 1 ] || [ $# -gt 3 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./build_dpdk.sh <dpdk_version> [<arch> [<ncores>]]\" where arch is any gcc/clang compatible -march and ncores could be any number or empty for all"
        exit 1
    fi

    local dpdk_version=$1
    local arch="${2:-native}"
    local ncores="${3:-$(nproc)}"

    cd /tmp
    curl -L "https://fast.dpdk.org/rel/dpdk-${dpdk_version}.tar.xz" | tar xJf -
    cd dpdk*"${dpdk_version}"
    meson setup build --prefix "/opt/dpdk/${dpdk_version}" -Dcpu_instruction_set="${arch}"
    ninja -j"${ncores}" -C build install

}

main "$@"

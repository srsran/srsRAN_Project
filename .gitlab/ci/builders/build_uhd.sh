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
        echo >&2 "Run like this: \"./build_uhd.sh <uhd_version> [<arch> [<ncores>]]\" where arch is any gcc/clang compatible -march and ncores could be any number or empty for all"
        exit 1
    fi

    local uhd_version=$1
    local arch="${2:-native}"
    local ncores="${3:-$(nproc)}"

    cd /tmp
    curl -L "https://github.com/EttusResearch/uhd/archive/refs/tags/v${uhd_version}.tar.gz" | tar xzf -

    cd uhd*"${uhd_version}"/host && mkdir -p build && cd build
    cmake \
        -DCMAKE_INSTALL_PREFIX=/opt/uhd/"${uhd_version}" \
        -DENABLE_LIBUHD=On \
        -DENABLE_PYTHON_API=Off \
        -DENABLE_EXAMPLES=Off \
        -DENABLE_TESTS=Off \
        -DCMAKE_CXX_FLAGS="-march=${arch}" ..
    make -j"${ncores}"
    make -j"${ncores}" install
}

main "$@"

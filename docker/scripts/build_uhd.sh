#!/bin/bash
#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
    cmake --build . -- -j"${ncores}"
    cmake --install .

}

main "$@"

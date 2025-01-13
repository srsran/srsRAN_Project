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
    meson compile -j "${ncores}" -C build
    meson install -C build
}

main "$@"

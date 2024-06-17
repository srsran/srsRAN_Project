#!/bin/bash
#
# Copyright 2021-2024 Software Radio Systems Limited
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

set -e

main() {

    # Check number of args
    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_dependencies.sh [<mode>]\" where mode could be: build, run and extra"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    . /etc/os-release

    echo "== Installing srsRAN dependencies, mode $mode =="

    if [[ "$ID" == "debian" || "$ID" == "ubuntu" ]]; then
        if [[ "$mode" == "all" || "$mode" == "build" ]]; then
            DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
                cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
        fi
        if [[ "$mode" == "all" || "$mode" == "run" ]]; then
            DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
                libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
        fi
        if [[ "$mode" == "all" || "$mode" == "extra" ]]; then
            DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
                libzmq3-dev libuhd-dev uhd-host libboost-program-options-dev libdpdk-dev libelf-dev libdwarf-dev
        fi

    elif [[ "$ID" == "arch" ]]; then
        if [[ "$mode" == "all" || "$mode" == "build" ]]; then
            pacman -Syu --noconfirm cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest pkgconf
        fi
        if [[ "$mode" == "all" || "$mode" == "run" ]]; then
            pacman -Syu --noconfirm fftw mbedtls yaml-cpp lksctp-tools gtest
        fi
        if [[ "$mode" == "all" || "$mode" == "extra" ]]; then
            pacman -Syu --noconfirm zeromq libuhd boost dpdk libelf libdwarf
        fi

    elif [[ "$ID" == "rhel" ]]; then
        if [[ "$mode" == "all" || "$mode" == "build" ]]; then
            dnf -y install cmake fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gcc-toolset-11 gcc-toolset-11-gcc-c++ gcc-toolset-12-libatomic-devel
        fi
        if [[ "$mode" == "all" || "$mode" == "run" ]]; then
            dnf -y install fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gcc-toolset-12-libatomic-devel
        fi
        if [[ "$mode" == "all" || "$mode" == "extra" ]]; then
            dnf -y install cppzmq-devel libusb1-devel boost-devel numactl-devel # dpdk libelf libdwarf
        fi

    else
        echo "OS $ID not supported"
        exit 1
    fi

}

main "$@"

#!/bin/bash
#
# This script will install srsran dependencies
#
# Run like this: ./install_dependencies.sh [<mode>]
# E.g.: ./install_dependencies
# E.g.: ./install_dependencies build
# E.g.: ./install_dependencies run
# E.g.: ./install_dependencies extra
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

    fi

}

main "$@"

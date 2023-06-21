#!/bin/bash

#
# This script will install srsran dependencies
#
# Run like this: ./install_dependencies.sh
# E.g.: ./install_dependencies
#

main() {

    . /etc/os-release

    if [[ "$ID" == "debian" || "$ID" == "ubuntu" ]]; then
        DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
            cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
    elif [[ "$ID" == "arch" ]]; then
        pacman -Syu --noconfirm cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest pkgconf
    elif [[ "$ID" == "rhel" ]]; then
        dnf -y install cmake fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gcc-toolset-11 gcc-toolset-11-gcc-c++ 
        cd /tmp && git clone https://github.com/google/googletest.git &&
            cd googletest &&
            mkdir build &&
            cd build &&
            cmake .. &&
            make -j $(nproc) && make -j $(nproc) install &&
            cd ../../ && rm -rf googletest
    fi

}

main "$@"

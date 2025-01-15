#!/bin/bash
#
# Copyright 2013-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set -e # stop executing after error

main() {

    # Check number of args
    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_uhd_dependencies.sh [<mode>]\" where mode could be: build, run and all"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    # shellcheck source=/dev/null
    . /etc/os-release

    if [[ "$ID" == "debian" || "$ID" == "ubuntu" ]]; then
        if [[ "$mode" == "all" || "$mode" == "build" ]]; then
            DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
                curl apt-transport-https ca-certificates xz-utils \
                cmake build-essential pkg-config \
                libboost-all-dev libusb-1.0-0-dev \
                python3-mako python3-numpy python3-setuptools python3-requests

        fi
        if [[ "$mode" == "all" || "$mode" == "run" ]]; then
            DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends \
                cpufrequtils inetutils-tools libboost-all-dev libncurses5-dev libusb-1.0-0 libusb-1.0-0-dev \
                libusb-dev python3-dev python3-requests &&
                apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*
            uhd_images_downloader
        fi
    else
        echo "OS $ID not supported"
        exit 1
    fi

}

main "$@"

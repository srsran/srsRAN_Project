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

# Print help and syntax
print_help() {
    echo "Script for building srsGNB"
    echo
    echo "Syntax: builder.sh -args /folder"
    echo "args:"
    echo "  -h | --help                 - Print his message"
    echo "  -b | --build-folder         - Build folder. default \$workdir/build"
    echo "  -x | --cache-folder         - Cache folder. default \$workdir/ccache"
    echo "  -r | --clean-build          - Clean build folder before build. default True"
    echo "  -c | --compiler             - Compiler tool (gcc or clang)"
    echo "  -m | --make                 - Args to send to make tool"
    echo "  -u | --uhd                  - UHD version"
    echo "  -d | --dpdk                 - DPDK version"
    echo "  others                      - Extra args will be sent to 'cmake'"
    echo "folder:"
    echo "  Folder where source code is located. It should be the last parameter"
    echo ""
}

# Variables
FOLDER=""
BUILD_FOLDER="build"
CACHE_FOLDER="ccache"
CLEAN_BUILD="True"
MAKE_EXTRA="-j$(nproc)"
COMPILER="gcc"
UHD_VERSION=""
DPDK_VERSION=""

# Begin the parsing
while (("$#")); do
    case "$1" in
    -h | --help)
        print_help
        exit 0
        ;;
    -b | --build-folder)
        if [ -n "$2" ]; then
            BUILD_FOLDER="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -r | --clean-build)
        if [ -n "$2" ]; then
            CLEAN_BUILD="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -m | --make)
        if [ -n "$2" ]; then
            MAKE_EXTRA="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -x | --cache-folder)
        if [ -n "$2" ]; then
            CACHE_FOLDER="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -c | --compiler)
        if [ -n "$2" ]; then
            COMPILER=$(echo "$2" | tr '[:upper:]' '[:lower:]')
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -u | --uhd)
        if [ -n "$2" ]; then
            UHD_VERSION="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    -d | --dpdk)
        if [ -n "$2" ]; then
            DPDK_VERSION="$2"
            shift 2
        else
            echo "Error: Argument for $1 is missing" >&2
            echo "" >&2
            print_help
            exit 1
        fi
        ;;
    *)
        break
        ;;
    esac
done
FOLDER="${*: -1}"
set -- "${@:1:$#-1}"

case $COMPILER in
gcc)
    CC=$(which gcc)
    CXX=$(which g++)
    ;;
clang)
    CC=$(which clang)
    CXX=$(which clang++)
    ;;
*)
    echo "Error: Invalid compiler $COMPILER" >&2
    echo "" >&2
    print_help
    exit 1
    ;;
esac

export CC
export CXX

cd "$FOLDER" || exit

# Setup UHD
if [[ -n "$UHD_VERSION" ]]; then
    uhd_system_version=$(uhd_config_info --version) && regex="^UHD ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*" && [[ $uhd_system_version =~ $regex ]]
    uhd_system_version="${BASH_REMATCH[1]}"
    if [[ -z "$UHD_VERSION" || "$UHD_VERSION" == "$uhd_system_version" ]]; then
        echo "Using default UHD $uhd_system_version"
    else
        [ ! -d "/opt/uhd/$UHD_VERSION" ] && echo "UHD version not found" && exit 1
        export UHD_DIR="/opt/uhd/$UHD_VERSION"
        echo "UHD_DIR set to $UHD_DIR"
    fi
fi

# Setup DPDK
if [[ -n "$DPDK_VERSION" ]]; then
    [ ! -d "/opt/dpdk/$DPDK_VERSION" ] && echo "DPDK version not found" && exit 1
    # Create alias for _avx2 / _avx512 versions
    if [[ $DPDK_VERSION == *_* ]]; then
        DPDK_VERSION_BASE=${DPDK_VERSION%_*}
        # If the folder without _ exists, try to remove it
        if [ -e "/opt/dpdk/$DPDK_VERSION_BASE" ]; then
            rm -Rf "/opt/dpdk/$DPDK_VERSION_BASE" || echo "/opt/dpdk/$DPDK_VERSION_BASE already exists!!"
        fi
        # If we can create the hard line, we change DPDK_VERSION to the new version without _
        if ln -s "/opt/dpdk/$DPDK_VERSION" "/opt/dpdk/$DPDK_VERSION_BASE"; then
            DPDK_VERSION=$DPDK_VERSION_BASE
        fi
    fi
    export DPDK_DIR="/opt/dpdk/$DPDK_VERSION"
    echo "DPDK_DIR set to $DPDK_DIR"
fi

ARCH=$(uname -m)
if [[ "$ARCH" == "aarch64" ]]; then
    if ARMPL_DIR=$(ls -d /opt/arm/armpl_* 2>/dev/null); then
        export ARMPL_DIR
    fi
fi

# Setup cache dir
mkdir -p "$CACHE_FOLDER"
export CCACHE_BASEDIR=${PWD}
export CCACHE_DIR=${PWD}/${CACHE_FOLDER}
export CCACHE_COMPILERCHECK=content

# Clean build dir
if [[ "$CLEAN_BUILD" == "True" ]]; then
    echo "Cleaning build directory: $BUILD_FOLDER"
    rm -Rf "$BUILD_FOLDER"
else
    echo "Skip cleaning of build directory: $BUILD_FOLDER"
fi
# Use ccache if available
(which ccache >/dev/null) && CCACHE_CMAKE_ARGS="-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache" || CCACHE_CMAKE_ARGS=""

# Build process
ccache -z || true
mkdir -p "$BUILD_FOLDER"
cd "$BUILD_FOLDER" || exit

# First cmake call to setup. Saving output to later get targets if required
cmake $CCACHE_CMAKE_ARGS "$@" .. 2>&1 | tee cmake_output.log

# If no targets are provided, get the targets from the cmake output
# This scripts considers a target each word in MAKE_ARGS that doesn't start with an hyphen
if [[ "$MAKE_EXTRA" =~ (^|[[:space:]])[^-][^[:space:]]* ]]; then
    :
else
    TARGETS=$(awk '/-- Adding binary target:/ {print substr($0, index($0,"target:")+7)}' cmake_output.log | tr '\n' ' ')
    MAKE_EXTRA="$MAKE_EXTRA $TARGETS all"
fi

found=false
for word in $MAKE_EXTRA; do
  if [[ "$word" =~ ^[^-] ]]; then
    echo "-- Selecting target $word"
    found=true
  fi
done

if [[ $found == false ]]; then
  echo "-- Selecting default target"
fi

# Second cmake call to build
cmake --build . -- $MAKE_EXTRA
ccache -sv 2>/dev/null || true

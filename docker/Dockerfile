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

# Build args
################
# OS_VERSION            Ubuntu OS version
# LIB                   uhd or dpdk (lowercase)
# LIB_VERSION           UHD or DPDK version number
# MARCH                 gcc/clang compatible arch
# NUM_JOBS             Number or empty for all
# EXTRA_CMAKE_ARGS      Extra flags for srsRAN Project

ARG OS_VERSION=24.04
ARG LIB=uhd
ARG LIB_VERSION=4.7.0.0
ARG MARCH=native
ARG NUM_JOBS=""

##################
# Stage 1: Build #
##################
FROM ubuntu:$OS_VERSION AS builder

# Adding the complete repo to the context, in /src folder
ADD . /src
# An alternative could be to download the repo
# RUN apt update && apt-get install -y --no-install-recommends git git-lfs
# RUN git clone https://github.com/srsran/srsRAN_Project.git /src

# Install srsRAN build dependencies
ARG LIB
RUN /src/docker/scripts/install_dependencies.sh build && \
    /src/docker/scripts/install_${LIB}_dependencies.sh build && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git

ARG LIB_VERSION
ARG MARCH
ARG NUM_JOBS

# Compile UHD/DPDK
RUN /src/docker/scripts/build_${LIB}.sh ${LIB_VERSION} ${MARCH} ${NUM_JOBS}

# Compile srsRAN Project and install it in the OS
ARG EXTRA_CMAKE_ARGS=""
RUN if [ -z "$NUM_JOBS" ]; then NUM_JOBS=$(nproc); fi && \
    LIB_UPPER=$(echo $LIB | tr '[:lower:]' '[:upper:]') && \
    export ${LIB_UPPER}_DIR="/opt/${LIB}/${LIB_VERSION}" && \
    if [ "$LIB_UPPER" = "UHD" ]; then TARGETS="srsdu_split_8 gnb_split_8"; else TARGETS="srsdu_split_7_2 gnb_split_7_2"; fi \
    && \
    /src/docker/scripts/builder.sh -m "-j${NUM_JOBS} srscu $TARGETS ru_emulator" \
    -DBUILD_TESTS=False -DENABLE_${LIB_UPPER}=On -DMARCH=${MARCH} -DCMAKE_INSTALL_PREFIX=/opt/srs \
    ${EXTRA_CMAKE_ARGS} /src \
    && \
    mkdir -p /opt/srs/bin /opt/srs/share/srsran && \
    cp /src/build/apps/cu/srscu                 /opt/srs/bin/srscu       && \
    cp /src/build/apps/du_split_*/srsdu         /opt/srs/bin/srsdu       && \
    cp /src/build/apps/gnb_split_*/gnb          /opt/srs/bin/gnb         && \
    cp /src/build/apps/examples/ofh/ru_emulator /opt/srs/bin/ru_emulator && \
    cp /src/configs/*.yml /opt/srs/share/srsran/

################
# Stage 2: Run #
################

FROM ubuntu:$OS_VERSION

ARG LIB
ARG LIB_VERSION

# Copy srsRAN binaries and libraries installed in previous stage
COPY --from=builder /opt/${LIB}/${LIB_VERSION} /opt/${LIB}/${LIB_VERSION}
COPY --from=builder /opt/srs /usr/local

# Copy the install dependencies scripts
ADD docker/scripts/install_${LIB}_dependencies.sh /usr/local/etc/install_lib_dependencies.sh
ADD docker/scripts/install_dependencies.sh /usr/local/etc/install_srsran_dependencies.sh
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/${LIB}/${LIB_VERSION}/lib/:/opt/${LIB}/${LIB_VERSION}/lib/x86_64-linux-gnu/:/opt/${LIB}/${LIB_VERSION}/lib/aarch64-linux-gnu/
ENV PATH=$PATH:/opt/${LIB}/${LIB_VERSION}/bin/

# Install srsran and lib runtime dependencies
RUN /usr/local/etc/install_srsran_dependencies.sh run && \
    /usr/local/etc/install_lib_dependencies.sh run && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

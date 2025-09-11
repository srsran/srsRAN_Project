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
# UHD_VERSION           UHD version number
# DPDK_VERSION          DPDK version number
# MARCH                 gcc/clang compatible arch
# NUM_JOBS              Number or empty for all
# EXTRA_CMAKE_ARGS      Extra flags for srsRAN Project

ARG OS_VERSION=24.04
ARG UHD_VERSION=4.7.0.0
ARG DPDK_VERSION=24.11.2
ARG MARCH=native
ARG NUM_JOBS=""

##################
# Stage 1: Build #
##################

FROM ubuntu:$OS_VERSION AS builder

# Adding the complete repo to the context, in /src folder
ADD . /src
# An alternative could be to download the repo
# RUN apt update && apt-get install -y --no-install-recommends git git-lfs ca-certificates
# RUN git clone https://github.com/srsran/srsRAN_Project.git /src

# Install srsRAN build dependencies
RUN /src/docker/scripts/install_dependencies.sh build && \
    /src/docker/scripts/install_uhd_dependencies.sh build && \
    /src/docker/scripts/install_dpdk_dependencies.sh build && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git clang

ARG UHD_VERSION
ARG DPDK_VERSION
ARG MARCH
ARG NUM_JOBS

# Compile UHD/DPDK
RUN /src/docker/scripts/build_uhd.sh "${UHD_VERSION}" ${MARCH} ${NUM_JOBS} && \
    /src/docker/scripts/build_dpdk.sh "${DPDK_VERSION}" ${MARCH} ${NUM_JOBS}

# Compile srsRAN Project and install it in the OS
ARG COMPILER=gcc
ARG EXTRA_CMAKE_ARGS=""
ENV UHD_DIR=/opt/uhd/${UHD_VERSION}
ENV DPDK_DIR=/opt/dpdk/${DPDK_VERSION}
RUN if [ -z "$NUM_JOBS" ]; then NUM_JOBS=$(nproc); fi \
    && \
    /src/docker/scripts/builder.sh -c ${COMPILER} -m "-j${NUM_JOBS} srscu srsdu srsdu_split_8 srsdu_split_7_2 gnb gnb_split_8 gnb_split_7_2 ru_emulator" \
    -DBUILD_TESTING=False -DENABLE_UHD=On -DENABLE_DPDK=On -DMARCH=${MARCH} -DCMAKE_INSTALL_PREFIX=/opt/srs \
    ${EXTRA_CMAKE_ARGS} /src
RUN cp /src/build/apps/cu/srscu             /tmp/srscu                     && \
    cp /src/build/apps/du/srsdu             /tmp/srsdu                     && \
    cp /src/build/apps/du_split_8/srsdu     /tmp/srsdu_split_8             && \
    cp /src/build/apps/du_split_7_2/srsdu   /tmp/srsdu_split_7_2           && \
    cp /src/build/apps/gnb/gnb              /tmp/gnb                       && \
    cp /src/build/apps/gnb_split_8/gnb      /tmp/gnb_split_8               && \
    cp /src/build/apps/gnb_split_7_2/gnb    /tmp/gnb_split_7_2             && \
    cd /src/build                                                          && \
    make install                                                           && \
    mv /tmp/srscu                           /opt/srs/bin/srscu             && \
    mv /tmp/srsdu                           /opt/srs/bin/srsdu             && \
    mv /tmp/srsdu_split_8                   /opt/srs/bin/srsdu_split_8     && \
    mv /tmp/srsdu_split_7_2                 /opt/srs/bin/srsdu_split_7_2   && \
    mv /tmp/gnb                             /opt/srs/bin/gnb               && \
    mv /tmp/gnb_split_8                     /opt/srs/bin/gnb_split_8       && \
    mv /tmp/gnb_split_7_2                   /opt/srs/bin/gnb_split_7_2

################
# Stage 2: Run #
################

FROM ubuntu:$OS_VERSION AS runtime

ARG UHD_VERSION
ARG DPDK_VERSION

# Copy srsRAN binaries and libraries installed in previous stage
COPY --from=builder /opt/uhd/${UHD_VERSION}   /opt/uhd/${UHD_VERSION}
COPY --from=builder /opt/dpdk/${DPDK_VERSION} /opt/dpdk/${DPDK_VERSION}
COPY --from=builder /opt/srs                  /usr/local

# Copy the install dependencies scripts
ADD docker/scripts/install_uhd_dependencies.sh  /usr/local/etc/install_uhd_dependencies.sh
ADD docker/scripts/install_dpdk_dependencies.sh /usr/local/etc/install_dpdk_dependencies.sh
ADD docker/scripts/install_dependencies.sh      /usr/local/etc/install_srsran_dependencies.sh
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/uhd/${UHD_VERSION}/lib/:/opt/uhd/${UHD_VERSION}/lib/x86_64-linux-gnu/:/opt/uhd/${UHD_VERSION}/lib/aarch64-linux-gnu/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/dpdk/${DPDK_VERSION}/lib/:/opt/dpdk/${DPDK_VERSION}/lib/x86_64-linux-gnu/:/opt/dpdk/${DPDK_VERSION}/lib/aarch64-linux-gnu/
ENV PATH=$PATH:/opt/uhd/${UHD_VERSION}/bin/:/opt/dpdk/${DPDK_VERSION}/bin/

# Install srsran and lib runtime dependencies
RUN /usr/local/etc/install_srsran_dependencies.sh run && \
    /usr/local/etc/install_uhd_dependencies.sh run && \
    /usr/local/etc/install_dpdk_dependencies.sh run && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends curl ntpdate && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

#########################################################
# Stage 3: Generate container SPDX and merge build SPDX #
#########################################################

FROM anchore/syft:v1.26.1 AS syft-bin
FROM python:3.11.3-alpine AS spdx-merger
COPY --from=syft-bin  /syft                      /syft
COPY --from=runtime   /                          /rootfs
COPY --from=builder   /opt/srs/share/srsran.spdx /sbom/srsran.spdx

# Generate SPDX file for the container image
RUN mkdir -p /sbom /usr/local/etc/ && /syft dir:/rootfs -o spdx > /sbom/container.spdx
# Merge the container SPDX with the build SPDX
RUN pip install spdxmerge==0.2.0 && \
    sed -i -e '/def validate(self/,/return messages/ { /messages.append("ExtractedLicense text can not be None"/ { s#messages.append.*#return messages# } }' /usr/local/lib/python3.11/site-packages/spdx/license.py && \
    sed -i -e 's|def write_text_value(tag, value, out):|def write_text_value(tag, value, out):\n    if value is None:\n        return|'  /usr/local/lib/python3.11/site-packages/spdx/writers/tagvalue.py && \
    spdxmerge \
      --docpath /sbom \
      --outpath /usr/local/etc/ \
      --name "srsran-image" \
      --mergetype 1 \
      --author "Software Radio Systems Ltd" \
      --email "srs@srs.io" \
      --docnamespace "https://srs.io/spdxdocs/srsran" \
      --filetype T >/dev/null && \
    sed -i 's/^\(FilesAnalyzed:[[:space:]]*\)True/\1true/; s/^\(FilesAnalyzed:[[:space:]]*\)False/\1false/' /usr/local/etc/merged-SBoM-deep.spdx && \
    sed -i -E 's|^(LicenseID:.*)|\1\nExtractedText: NOASSERTION|' /usr/local/etc/merged-SBoM-deep.spdx && \
    sed -i '/^Relationship:.*#/d' /usr/local/etc/merged-SBoM-deep.spdx

######################################################
# Final Stage: Resume run stage with final SPDX file #
######################################################

FROM runtime
COPY --from=spdx-merger /usr/local/etc/merged-SBoM-deep.spdx /usr/local/etc/srsran_image.spdx

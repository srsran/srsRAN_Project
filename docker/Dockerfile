#
# Copyright 2021-2023 Software Radio Systems Limited
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

##################
# Stage 1: Build #
##################

ARG OS_VERSION=22.04
FROM ubuntu:$OS_VERSION as builder

# Adding the complete repo to the context, in /src folder
ADD . /src
# An alternative could be to download the repo
# RUN apt update && apt-get install -y --no-install-recommends git git-lfs
# RUN git clone https://github.com/srsran/srsRAN_Project.git /src

# Install build dependencies, and also ZMQ and UHD libraries
RUN /src/.gitlab/ci/builders/install_dependencies.sh

# Build srsRAN Project with default options
RUN /src/.gitlab/ci/builders/builder.sh /src
# Install in the OS filesystem to later copy into the next stage
RUN cd /src/build && make install

################
# Stage 2: Run #
################

FROM ubuntu:$OS_VERSION

# Copy srsRAN binaries and libraries installed in previous stage
COPY --from=builder /usr/local /usr/local

# Copy the install dependencies script
ADD .gitlab/ci/builders/install_dependencies.sh /usr/local/bin

# Install runtime dependencies and extra libs such as UHD/ZMQ
RUN /usr/local/bin/install_dependencies.sh run && \
    /usr/local/bin/install_dependencies.sh extra && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

# Uncomment this to download uhd images inside the container 
# instead of sharing them with the host (in the docker-compose.yml)
# RUN uhd_images_downloader -t b2xx && \
#     uhd_images_downloader -t n300    

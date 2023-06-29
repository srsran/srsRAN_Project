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

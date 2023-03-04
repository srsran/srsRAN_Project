FROM ubuntu:22.04

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends \
     cmake \
     make \
     gcc \
     git \
     g++ \
     pkg-config \
     libboost1.74-dev \
     libfftw3-dev \
     libmbedtls-dev \
     libsctp-dev \
     libyaml-cpp-dev \
     libgtest-dev \
     libuhd-dev \
     openssh-client \
     && rm -rf /var/lib/apt/lists/*
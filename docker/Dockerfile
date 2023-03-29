#---------------------------------------------------------------------
# BUILDER IMAGE
#---------------------------------------------------------------------
ARG BASE_IMAGE=ubuntu:focal

FROM $BASE_IMAGE as builder

ARG UHD_VERSION=v4.3.0.0

# Install dependencies for srsRAN and UHD build
RUN apt update && \
     DEBIAN_FRONTEND=noninteractive apt install -y \
     build-essential \
     cmake libfftw3-dev \
     libmbedtls-dev \
     libboost-program-options-dev \
     libconfig++-dev \
     libyaml-cpp-dev \
     libgtest-dev \
     git-lfs \
     libsctp-dev \
     libzmq3-dev \
     iputils-ping iproute2 iptables git wget \
     libboost-all-dev libusb-1.0-0-dev \
     doxygen python3-docutils python3-mako \
     python3-numpy python3-requests \
     python3-ruamel.yaml python3-setuptools && \
     rm -rf /var/lib/apt/lists/*

# Build UHD driver
RUN git clone https://github.com/EttusResearch/uhd.git ~/uhd && \
    cd ~/uhd && git checkout $UHD_VERSION && cd host && mkdir build && \
    cd build && cmake ../ && make -j $(nproc) && make install && ldconfig

# COPY . /srsRAN # Build local source
# Build srsRAN
RUN git clone https://github.com/srsran/srsRAN_Project.git /srsRAN_Project && \
    cd srsRAN_Project && \ 
    mkdir build &&  cd build && \
    cmake  ../ && \
    make -j $(nproc)

# Pull fpga images for currently validated USRPs
WORKDIR /images/
RUN uhd_images_downloader -t b2xx -i /srsran/bin -i /images/ && \
    uhd_images_downloader -t n300 -i /srsran/bin -i /images/    
#---------------------------------------------------------------------
# TARGET IMAGE
#---------------------------------------------------------------------
ARG BASE_IMAGE=ubuntu:jammy
FROM $BASE_IMAGE as srsran

ARG UHD_VERSION=v4.3.0.0

RUN apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y \ 
    iproute2 net-tools iputils-ping -y

COPY --from=builder /usr/local/lib/libsrsran_* \
                    /usr/local/lib/libuhd.so.* \
                    /usr/local/lib/

# Copy shared libraries
COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libmbedcrypto.so.3 \ 
                    /usr/lib/x86_64-linux-gnu/libconfig++.so.9 \
                    /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_regex.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_serialization.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libusb-1.0.so.0 \
                    /usr/lib/x86_64-linux-gnu/libsctp.so.1 \
                    /usr/lib/x86_64-linux-gnu/libudev.so.1 \
                    /usr/lib/x86_64-linux-gnu/libicuuc.so.66 \
                    /usr/lib/x86_64-linux-gnu/libicui18n.so.66 \
                    /usr/lib/x86_64-linux-gnu/libicudata.so.66 \
                    /usr/lib/x86_64-linux-gnu/libyaml-cpp.so.0.6 \ 
                    /usr/lib/x86_64-linux-gnu/libdl.so.2 \
                    # UHD_VERSION=v4.3.0.0
                    /usr/lib/x86_64-linux-gnu/libpython3.8.so.1.0 \
                    /usr/lib/x86_64-linux-gnu/libexpat.so.1 \
                    # /usr/local/lib/libsrsran_rf_zmq.so.0 \
                    /usr/lib/x86_64-linux-gnu/libfftw3f.so.3 /usr/lib/x86_64-linux-gnu/
RUN ldconfig

# Copy fpga images
WORKDIR /srsran/images/
COPY --from=builder /images/* /srsran/images/ 

# Copy config files
WORKDIR /srsran/etc
COPY docker/gnb_config.yaml .
# Copy srsRAN binaries
WORKDIR /srsran/bin
COPY docker/entrypoint.sh .
COPY --from=builder /srsRAN_Project/build/apps/gnb/gnb .

# Trigger entrypoint script
ENTRYPOINT ["/srsran/bin/entrypoint.sh"]

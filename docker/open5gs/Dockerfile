ARG OS_VERSION=22.04
FROM ubuntu:$OS_VERSION as base

ENV PYTHONBUFFERED=1
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt install -y software-properties-common \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
    && apt-get install -y \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    ninja-build \
    build-essential \
    flex \
    bison \
    git \
    libsctp-dev \
    libgnutls28-dev \
    libgcrypt-dev \
    libssl-dev \
    libidn11-dev \
    libmongoc-dev \
    libbson-dev \
    libyaml-dev \
    libnghttp2-dev \
    libmicrohttpd-dev \
    libcurl4-gnutls-dev \
    libnghttp2-dev \
    libtins-dev \
    meson \
    mongodb \
    curl \
    gettext \
    gdb \
    iproute2 \
    iptables \
    iputils-ping \
    netcat \
    iperf \
    iperf3 \
    libtalloc-dev \
    cmake

# To set a fixed version of open5gs use
ARG OPEN5GS_VERSION
RUN echo $OPEN5GS_VERSION > ./open5gsversion
# get latest open5gs tag (must be stored in a file, because docker does not allow to use the return value directly)
# RUN git ls-remote --tags https://github.com/open5gs/open5gs | sort -t '/' -k 3 -V | awk -F/ '{ print $3 }' | awk '!/\^\{\}/' | tail -n 1 > ./open5gsversion

# open5gs
RUN git clone --depth 1 --branch $(cat ./open5gsversion) https://github.com/open5gs/open5gs open5gs\
    && cd open5gs \
    && meson build --prefix=`pwd`/install \
    && ninja -C build \
    && cd build \
    && ninja install

# webgui
RUN curl -fsSL https://deb.nodesource.com/setup_14.x | bash - \
    && apt-get install -y nodejs \
    && cd open5gs/webui \
    && npm ci --no-optional

# mongodb python prerequisites
RUN python3 -m pip install pymongo click pyroute2 ipaddress python-iptables

FROM base as open5gs

WORKDIR /open5gs
COPY open5gs-5gc.yml open5gs-5gc.yml.in
COPY open5gs_entrypoint.sh .
COPY add_users.py .
COPY setup_tun.py .
# This tries to copy "subscriber_db.csv" but will not fail if the file doesn't exist
COPY subscriber_db.cs[v] .

ENV PATH="${PATH}:/open5gs/build/tests/app/"

ENTRYPOINT [ "./open5gs_entrypoint.sh" ]

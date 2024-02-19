#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG VERSION=22.04
ARG OS_NAME=ubuntu
FROM $OS_NAME:$VERSION

RUN TZ=Europe/Madrid && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    git git-lfs \
    clang llvm \
    ccache gcovr valgrind \
    python3-dev python3-venv && \
    (apt-get install -y --no-install-recommends libclang-rt-dev || true) && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/* && \
    git lfs install

ADD install_dependencies.sh builder.sh changed_tests.py ram_reporter.py /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/changed_tests.py /usr/local/bin/ram_reporter.py && \
    /usr/local/bin/install_dependencies.sh && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/* && \
    python3 -m venv /usr/local/builder_tools && \
    /usr/local/builder_tools/bin/pip install "pandas<3" "psutil"

ADD uhd /opt/uhd
ADD dpdk /opt/dpdk

WORKDIR /workdir

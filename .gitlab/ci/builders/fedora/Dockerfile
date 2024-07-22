#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG VERSION=39
ARG OS_NAME=fedora
FROM $OS_NAME:$VERSION

RUN TZ=Europe/Madrid && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    dnf update -y && \
    dnf install -y \
    which git git-lfs \
    clang llvm \
    ccache gcovr valgrind \
    python3-devel python3-virtualenv && \
    dnf install -y libstdc++-static && \
    dnf autoremove -y && dnf clean all && rm -rf /var/cache/dnf && \
    git lfs install

ADD install_dependencies.sh builder.sh changed_tests.py ram_reporter.py /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/changed_tests.py /usr/local/bin/ram_reporter.py && \
    /usr/local/bin/install_dependencies.sh && \
    dnf autoremove -y && dnf clean all && rm -rf /var/cache/dnf && \
    python3 -m venv /usr/local/builder_tools && \
    /usr/local/builder_tools/bin/pip install "pandas<3" "psutil"

WORKDIR /workdir

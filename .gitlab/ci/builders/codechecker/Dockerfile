#
# Copyright 2013-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG CR_REGISTRY_URI
ARG CI_TOOLS_REPO
ARG CODECHECKER_VERSION=1.0.1

FROM ${CR_REGISTRY_URI}/${CI_TOOLS_REPO}/codechecker:${CODECHECKER_VERSION}

ADD install_dependencies.sh /usr/local/bin
RUN chmod +x /usr/local/bin/install_dependencies.sh

RUN DEBIAN_FRONTEND=noninteractive apt-get update && install_dependencies.sh \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

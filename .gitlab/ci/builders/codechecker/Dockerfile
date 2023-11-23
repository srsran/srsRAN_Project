ARG CR_REGISTRY_URI
ARG CI_TOOLS_REPO
ARG CODECHECKER_VERSION=1.0.0

FROM ${CR_REGISTRY_URI}/${CI_TOOLS_REPO}/codechecker:${CODECHECKER_VERSION}

ADD install_dependencies.sh /usr/local/bin
RUN chmod +x /usr/local/bin/install_dependencies.sh

RUN DEBIAN_FRONTEND=noninteractive apt-get update && install_dependencies.sh \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

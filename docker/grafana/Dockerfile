#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG GRAFANA_VERSION=11.5.1
FROM grafana/grafana-oss:${GRAFANA_VERSION}

ADD provisioning /etc/grafana/provisioning
ADD dashboards   /etc/dashboards

USER root
ARG LOGO_SRC=https://raw.githubusercontent.com/srsran/srsRAN_Project_docs/main/docs/source/.imgs/logo.png
# If LOGO_SRC is a valid path inside the container, then move it to the correct location
# Otherwise, download it from the URL
RUN if [ -f "${LOGO_SRC}" ]; then \
        mv "${LOGO_SRC}" /usr/share/grafana/public/img/logo.png; \
    else \
        wget -O /usr/share/grafana/public/img/logo.png ${LOGO_SRC}; \
    fi
USER grafana
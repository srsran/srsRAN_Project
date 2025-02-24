#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG GRAFANA_VERSION=10.1.5
FROM grafana/grafana-oss:${GRAFANA_VERSION}

ADD provisioning /etc/grafana/provisioning
ADD dashboards   /etc/dashboards

USER root
ARG LOGO_URL=https://raw.githubusercontent.com/srsran/srsRAN_Project_docs/main/docs/source/.imgs/logo.png
RUN if [ -f "${LOGO_URL}" ]; then \
        mv "${LOGO_URL}" /usr/share/grafana/public/img/logo.png; \
    else \
        wget -O /usr/share/grafana/public/img/logo.png ${LOGO_URL}; \
    fi
USER grafana
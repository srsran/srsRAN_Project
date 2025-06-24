#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG GF_VERSION=12.0.2
FROM grafana/grafana-oss:${GF_VERSION}

ADD provisioning /etc/grafana/provisioning
ADD dashboards   /etc/dashboards

USER root
RUN rm -rf /usr/share/grafana/public/app/plugins/panel/table
ARG LOGO_SRC=https://raw.githubusercontent.com/srsran/srsRAN_Project_docs/main/docs/source/.imgs/logo.png
# If LOGO_SRC is a valid path inside the container, then move it to the correct location
# Otherwise, download it from the URL
RUN if [ -f "${LOGO_SRC}" ]; then \
        mv "${LOGO_SRC}" /usr/share/grafana/public/img/logo.png; \
    else \
        wget -O /usr/share/grafana/public/img/logo.png ${LOGO_SRC}; \
    fi
USER grafana
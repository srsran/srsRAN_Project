#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

ARG GF_VERSION=12.0.2
FROM grafana/grafana-oss:${GF_VERSION}

ADD provisioning /etc/grafana/provisioning
ADD dashboards   /usr/share/grafana/public/dashboards

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
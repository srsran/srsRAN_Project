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
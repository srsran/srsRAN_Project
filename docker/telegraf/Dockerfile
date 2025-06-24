#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

ARG TELEGRAF_VERSION=1.35.0
FROM telegraf:${TELEGRAF_VERSION}

ADD *.conf /etc/telegraf/
ADD /entrypoint.sh /usr/local/bin/entrypoint.sh

ENTRYPOINT [ "/usr/local/bin/entrypoint.sh" ]

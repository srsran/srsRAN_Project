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

ARG TELEGRAF_VERSION=1.35.0
FROM telegraf:${TELEGRAF_VERSION}
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends python3-websocket socat

ADD *.conf /etc/srs/
ADD entrypoint.sh ws_adapter.py /usr/local/bin/

ENTRYPOINT [ "/usr/local/bin/entrypoint.sh" ]

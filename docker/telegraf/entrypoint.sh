#!/usr/bin/env bash
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

set -o pipefail

if [ -n "$RETINA_PORTS" ]; then
  # In this mode, we expect to receive data over UDP, telling websocket ip/port of the server.
  export WS_URL=$(socat -u UDP-RECVFROM:"${RETINA_PORTS}",reuseaddr STDOUT)
fi
telegraf --config /etc/srs/telegraf.conf $TELEGRAF_CLI_EXTRA_ARGS &
child=$!

health_code=0
_term() {
    curl -sf -o /dev/null http://localhost:9273/health
    health_code=$?
    echo "$(date -u '+%Y-%m-%dT%H:%M:%SZ') I! Health check returned code $health_code"
    kill -TERM "$child" 2>/dev/null
}
trap _term SIGTERM SIGINT

wait "$child"

exit $health_code

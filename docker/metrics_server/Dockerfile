#
# Copyright 2021-2024 Software Radio Systems Limited
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

###################
# Package Builder #
###################
FROM python:3.12-alpine as builder

# Add the folder
ADD . /src
WORKDIR /src

# Build wheels and save dependencies
RUN pip wheel -w /src/wheelhouse /src

######################
# Final docker image #
######################
FROM python:3.12-alpine

# Avoid extra index url set by gitlab-ci
ARG PIP_EXTRA_INDEX_URL=""

# Copy wheels
COPY --from=builder /src/wheelhouse /app/

# Install
RUN python3 -m pip install /app/*.whl

WORKDIR /var/log/metrics_server

CMD metrics-server --port "${PORT}${RETINA_PORTS}" --bucket "${BUCKET}" --testbed "${TESTBED}" --db-config url="${URL}" org="${ORG}" token="${TOKEN}"

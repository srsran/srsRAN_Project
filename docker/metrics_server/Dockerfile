#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

###################
# Package Builder #
###################
FROM python:3.12-alpine AS builder

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

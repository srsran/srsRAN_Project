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

ARG VERSION=22.04
ARG OS_NAME=ubuntu
FROM $OS_NAME:$VERSION

ADD install_dependencies.sh /usr/local/bin
RUN chmod +x /usr/local/bin/install_dependencies.sh

RUN TZ=Europe/Madrid && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN install_dependencies.sh && apt-get install -y --no-install-recommends \
  git git-lfs \
  clang llvm \
  ccache gcovr valgrind libelf-dev libdwarf-dev \
  python3-dev python3-venv \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/* && git lfs install

RUN (DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y --no-install-recommends libclang-rt-dev || true) \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*

ADD builder.sh /usr/local/bin
RUN chmod +x /usr/local/bin/builder.sh

RUN python3 -m venv /usr/local/builder_tools
RUN /usr/local/builder_tools/bin/pip install "pandas<3" "psutil"
ADD changed_tests.py /usr/local/bin
RUN chmod +x /usr/local/bin/changed_tests.py
ADD ram_reporter.py /usr/local/bin
RUN chmod +x /usr/local/bin/ram_reporter.py

ADD uhd /opt/uhd

WORKDIR /workdir

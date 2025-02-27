#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES udp_cli11_schema.cpp
	udp_config_yaml_writer.cpp
)

add_library(srsran_network_app_helpers STATIC ${SOURCES})
target_include_directories(srsran_network_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})

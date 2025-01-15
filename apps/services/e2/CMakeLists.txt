#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        e2_cli11_schema.cpp
        e2_config_yaml_writer.cpp)

add_library(srsran_e2_app_service STATIC ${SOURCES})
target_include_directories(srsran_e2_app_service PRIVATE ${CMAKE_SOURCE_DIR})

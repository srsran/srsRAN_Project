#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
            fapi_config_cli11_schema.cpp
            fapi_config_translator.cpp
            fapi_config_yaml_writer.cpp)

add_library(srsran_fapi_app_unit STATIC ${SOURCES})
target_include_directories(srsran_fapi_app_unit PRIVATE ${CMAKE_SOURCE_DIR})

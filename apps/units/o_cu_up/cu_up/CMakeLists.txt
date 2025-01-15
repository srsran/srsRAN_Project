#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(metrics)

set(SOURCES
        cu_up_unit_config_cli11_schema.cpp
        cu_up_unit_config_translators.cpp
        cu_up_unit_config_validator.cpp
        cu_up_unit_config_yaml_writer.cpp)

add_library(srsran_cu_up_app_unit STATIC ${SOURCES})
target_include_directories(srsran_cu_up_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_cu_up_app_unit srsran_cu_up_unit_metrics_helpers)

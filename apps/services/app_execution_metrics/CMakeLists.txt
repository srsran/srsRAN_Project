#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        executor_metrics_config_cli11_schema.cpp
        executor_metrics_config_yaml_writer.cpp
        metrics/app_execution_metrics_builder.cpp)

add_library(srsran_exec_metrics_app_service STATIC ${SOURCES})
target_include_directories(srsran_exec_metrics_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_exec_metrics_app_service PRIVATE srsran_metrics_app_helpers srsran_metrics_json_generators)

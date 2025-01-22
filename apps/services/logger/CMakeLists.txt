#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_metrics_logger_utils STATIC metrics_logger_appconfig_cli11_schema.cpp metrics_logger_appconfig_yaml_writer.cpp)
target_include_directories(srsran_metrics_logger_utils PRIVATE ${CMAKE_SOURCE_DIR})

set(SOURCES
        logger_appconfig_cli11_schema.cpp
        logger_appconfig_validator.cpp
        logger_appconfig_yaml_writer.cpp)

add_library(srsran_logger_app_service STATIC ${SOURCES})
target_include_directories(srsran_logger_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_logger_app_service PRIVATE srsran_metrics_logger_utils)


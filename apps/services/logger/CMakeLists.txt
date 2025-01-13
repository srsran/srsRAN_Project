#
# Copyright 2021-2025 Software Radio Systems Limited
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



add_library(srsran_metrics_logger_utils STATIC metrics_logger_appconfig_cli11_schema.cpp metrics_logger_appconfig_yaml_writer.cpp)
target_include_directories(srsran_metrics_logger_utils PRIVATE ${CMAKE_SOURCE_DIR})

set(SOURCES
        logger_appconfig_cli11_schema.cpp
        logger_appconfig_validator.cpp
        logger_appconfig_yaml_writer.cpp)

add_library(srsran_logger_app_service STATIC ${SOURCES})
target_include_directories(srsran_logger_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_logger_app_service PRIVATE srsran_metrics_logger_utils)


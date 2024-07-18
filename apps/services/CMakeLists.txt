#
# Copyright 2021-2024 Software Radio Systems Limited
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

add_subdirectory(buffer_pool)
add_subdirectory(logger)

set(SOURCES
        metrics_log_helper.cpp
        e2_metric_connector_manager.cpp
        metrics_hub.cpp
        metrics_plotter_json.cpp
        metrics_plotter_stdout.cpp
        rlc_metrics_plotter_json.cpp
        stdin_command_dispatcher.cpp
        worker_manager.cpp)

add_library(srsran_app_services STATIC ${SOURCES})
target_include_directories(srsran_app_services PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_app_services srsran_logger_app_service srsran_buffer_pool_app_service)

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

add_subdirectory(metrics)

set(SOURCES
        flexible_o_du_factory.cpp
        flexible_o_du_impl.cpp
        flexible_o_du_metrics_aggregator.cpp
        commands/ntn_config_update_remote_command_factory.cpp)

add_library(srsran_flexible_o_du_helpers STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_helpers
        srsran_o_du
        srsran_pcap
        srsran_app_services
        srsran_o_du_low_unit_helpers
        srsran_o_du_high_unit_helpers
        srsran_flexible_o_du_unit_metrics_helpers)

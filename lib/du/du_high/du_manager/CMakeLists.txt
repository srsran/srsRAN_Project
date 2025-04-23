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

add_subdirectory(cbs)
add_subdirectory(converters)
add_subdirectory(du_ue)
add_subdirectory(procedures)
add_subdirectory(ran_resource_management)

set(SOURCES
        du_cell_manager.cpp
        du_manager_factory.cpp
        du_manager_impl.cpp
        du_positioning_handler_factory.cpp
        du_metrics_aggregator_impl.cpp
)

add_library(srsran_du_manager STATIC ${SOURCES})
target_link_libraries(srsran_du_manager
        du_ue
        du_resource_manager
        du_manager_converters
        srsran_du_config_validators
        mac_configuration_helpers
        srsran_rlc
        srsran_f1u_du
        srsran_gtpu)

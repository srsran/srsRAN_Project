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

set(SOURCES serving_cell_config_factory.cpp
        logical_channel_config_factory.cpp
        sched_cell_config_helpers.cpp
        scheduler_cell_config_validator.cpp
        scheduler_ue_config_validator.cpp
        serving_cell_config_validator.cpp
        csi_helper.cpp)
add_library(sched_config STATIC ${SOURCES})
target_link_libraries(sched_config srsran_ran du_resource_manager)

add_library(sched_config_manager OBJECT cell_configuration.cpp ue_configuration.cpp sched_config_manager.cpp)
target_link_libraries(sched_config_manager srsran_ran)

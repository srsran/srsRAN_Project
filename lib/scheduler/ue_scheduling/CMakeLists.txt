#
# Copyright 2021-2023 Software Radio Systems Limited
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

set(SOURCES
        harq_process.cpp
        ue_configuration.cpp
        dl_logical_channel_manager.cpp
        ul_logical_channel_manager.cpp
        ta_manager.cpp
        ue_channel_state_manager.cpp
        ue_link_adaptation_controller.cpp
        ue.cpp
        ue_cell.cpp
        ue_cell_grid_allocator.cpp
        ue_event_manager.cpp
        ue_srb0_scheduler.cpp
        ue_scheduler_impl.cpp
        ue_repository.cpp)

add_library(ue_sched OBJECT ${SOURCES})

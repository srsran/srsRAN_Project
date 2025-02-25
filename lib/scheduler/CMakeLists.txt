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

add_subdirectory(config)
add_subdirectory(support)
add_subdirectory(logging)
add_subdirectory(cell)
add_subdirectory(common_scheduling)
add_subdirectory(ue_context)
add_subdirectory(ue_scheduling)

set(SOURCES
        pdcch_scheduling/pdcch_resource_allocator_impl.cpp
        pdcch_scheduling/pdcch_slot_resource_allocator.cpp
        policy/scheduler_policy_factory.cpp
        policy/scheduler_time_rr.cpp
        policy/scheduler_time_qos.cpp
        pucch_scheduling/pucch_allocator_impl.cpp
        pucch_scheduling/pucch_guardbands_scheduler.cpp
        pucch_scheduling/pucch_resource_manager.cpp
        uci_scheduling/uci_scheduler_impl.cpp
        uci_scheduling/uci_allocator_impl.cpp
        slicing/inter_slice_scheduler.cpp
        slicing/ran_slice_instance.cpp
        slicing/slice_ue_repository.cpp
        srs/srs_scheduler_impl.cpp
        cell_scheduler.cpp
        scheduler_factory.cpp
        scheduler_impl.cpp
        support/csi_rs_helper.cpp
        support/pusch/pusch_td_resource_indices.cpp)

add_library(srsran_sched STATIC ${SOURCES}
        $<TARGET_OBJECTS:sched_config_manager>
        $<TARGET_OBJECTS:sched_cell>
        $<TARGET_OBJECTS:common_sched>
        $<TARGET_OBJECTS:ue_sched>)
target_link_libraries(srsran_sched srsran_ran sched_config sched_ue_context sched_support scheduler_logger)

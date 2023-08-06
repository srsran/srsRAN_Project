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

add_subdirectory(config)
add_subdirectory(logging)
add_subdirectory(cell)
add_subdirectory(common_scheduling)
add_subdirectory(ue_scheduling)

set(SOURCES
        pdcch_scheduling/pdcch_resource_allocator_impl.cpp
        policy/scheduler_policy_factory.cpp
        policy/scheduler_time_rr.cpp
        pucch_scheduling/pucch_allocator_impl.cpp
        pucch_scheduling/pucch_guardbands_scheduler.cpp
        pucch_scheduling/pucch_resource_manager.cpp
        uci_scheduling/uci_scheduler_impl.cpp
        uci_scheduling/uci_allocator_impl.cpp
        support/pdcch/pdcch_type0_helpers.cpp
        support/pdsch/pdsch_default_time_allocation.cpp
        support/pdsch/pdsch_dmrs_symbol_mask.cpp
        support/pucch/pucch_default_resource.cpp
        support/pusch/pusch_default_time_allocation.cpp
        support/pusch/pusch_dmrs_symbol_mask.cpp
        support/mcs_tbs_calculator.cpp
        support/tbs_calculator.cpp
        support/prbs_calculator.cpp
        support/mcs_calculator.cpp
        support/rb_helper.cpp
        support/dci_builder.cpp
        support/sch_pdu_builder.cpp
        support/csi_report_helpers.cpp
        cell_scheduler.cpp
        scheduler_factory.cpp
        scheduler_impl.cpp)

add_library(srsran_sched STATIC ${SOURCES}
        $<TARGET_OBJECTS:sched_cell>
        $<TARGET_OBJECTS:common_sched>
        $<TARGET_OBJECTS:ue_sched>)
target_link_libraries(srsran_sched srsran_ran sched_config scheduler_logger)

#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

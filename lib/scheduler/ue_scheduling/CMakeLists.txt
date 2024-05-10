#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        harq_process.cpp
        cell_harq_manager.cpp
        dl_logical_channel_manager.cpp
        ul_logical_channel_manager.cpp
        ta_manager.cpp
        ue_channel_state_manager.cpp
        ue_link_adaptation_controller.cpp
        ue.cpp
        ue_cell.cpp
        ue_cell_grid_allocator.cpp
        ue_event_manager.cpp
        ue_fallback_scheduler.cpp
        ue_scheduler_impl.cpp
        ue_repository.cpp)

add_library(ue_sched OBJECT ${SOURCES})

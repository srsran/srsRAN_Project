#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        harq_process.cpp
        ue_configuration.cpp
        dl_logical_channel_manager.cpp
        ul_logical_channel_manager.cpp
        ue.cpp
        ue_cell.cpp
        ue_cell_grid_allocator.cpp
        ue_event_manager.cpp
        ue_srb0_scheduler.cpp
        ue_scheduler_impl.cpp
        ue_dci_builder.cpp
        ue_sch_pdu_builder.cpp)

add_library(ue_sched OBJECT ${SOURCES})

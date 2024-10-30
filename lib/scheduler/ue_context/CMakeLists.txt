#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dl_logical_channel_manager.cpp
        ul_logical_channel_manager.cpp
        ta_manager.cpp
        ue_channel_state_manager.cpp
        ue_link_adaptation_controller.cpp
        ue.cpp
        ue_cell.cpp)

add_library(sched_ue_context OBJECT ${SOURCES})

#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ue_cell_grid_allocator.cpp
        ue_event_manager.cpp
        ue_fallback_scheduler.cpp
        ue_scheduler_impl.cpp
        ue_repository.cpp)

add_library(ue_sched OBJECT ${SOURCES})

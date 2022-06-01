#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES scheduler_prb.cpp ue/harq_process.cpp scheduler_impl.cpp scheduler_ssb.cpp cell/cell_configuration.cpp cell/resource_grid.cpp
        cell/ra_scheduler.cpp ue/ue.cpp ue/ue_configuration.cpp
        scheduler_factory.cpp cell/pdcch_scheduler_impl.cpp phy_helpers.cpp ue/ue_scheduler_impl.cpp
        ue/ue_event_manager.cpp ue/ue_cell_grid_allocator.cpp
        policy/scheduler_policy_factory.cpp policy/scheduler_time_rr.cpp)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched srsgnb_ran rrc_nr_asn1_helpers)

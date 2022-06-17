#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cell/cell_configuration.cpp
        cell/ra_scheduler.cpp
        cell/resource_grid.cpp
        cell/sib_scheduler.cpp
        pdcch_scheduler/pdcch_scheduler_impl.cpp
        policy/scheduler_policy_factory.cpp
        policy/scheduler_time_rr.cpp
        support/pdsch/pdsch_dmrs_symbol_mask.cpp
        support/tbs_calculator.cpp
        ue/harq_process.cpp
        ue/ue.cpp
        ue/ue_cell_grid_allocator.cpp
        ue/ue_configuration.cpp
        ue/ue_event_manager.cpp
        ue/ue_scheduler_impl.cpp
        phy_helpers.cpp
        scheduler_factory.cpp
        scheduler_impl.cpp
        scheduler_prb.cpp
        scheduler_ssb.cpp)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched srsgnb_ran rrc_nr_asn1_helpers)

#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cell/cell_configuration.cpp
        cell/resource_block_group.cpp
        cell/prb_grant.cpp
        common_scheduling/ra_scheduler.cpp
        cell/resource_grid.cpp
        common_scheduling/sib_scheduler.cpp
        pdcch_scheduling/pdcch_scheduler_impl.cpp
        policy/scheduler_policy_factory.cpp
        policy/scheduler_time_rr.cpp
        support/pdcch/pdcch_type0_css_coreset_config.cpp
        support/pdcch/pdcch_type0_css_occasions.cpp
        support/pdsch/pdsch_dmrs_symbol_mask.cpp
        support/tbs_calculator.cpp
        support/prbs_calculator.cpp
        ue_scheduling/harq_process.cpp
        ue_scheduling/ue.cpp
        ue_scheduling/ue_cell_grid_allocator.cpp
        ue_scheduling/ue_configuration.cpp
        ue_scheduling/ue_event_manager.cpp
        ue_scheduling/ue_scheduler_impl.cpp
        scheduler_factory.cpp
        scheduler_impl.cpp
        cell/scheduler_prb.cpp
        common_scheduling/ssb_scheduler.cpp)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched srsgnb_ran rrc_nr_asn1_helpers)

#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(config)
add_subdirectory(common_scheduling)
add_subdirectory(ue_scheduling)

set(SOURCES
        cell/cell_configuration.cpp
        cell/resource_block_group.cpp
        cell/prb_grant.cpp
        cell/resource_grid.cpp
        pdcch_scheduling/pdcch_resource_allocator_impl.cpp
        policy/scheduler_policy_factory.cpp
        policy/scheduler_time_rr.cpp
        pucch_scheduling/pucch_allocator_impl.cpp
        support/pdcch/pdcch_type0_css_occasions.cpp
        support/pdsch/pdsch_default_time_allocation.cpp
        support/pdsch/pdsch_dmrs_symbol_mask.cpp
        support/pucch/pucch_default_resource.cpp
        support/pusch/pusch_default_time_allocation.cpp
        support/pusch/pusch_dmrs_symbol_mask.cpp
        support/tbs_calculator.cpp
        support/prbs_calculator.cpp
        scheduler_factory.cpp
        scheduler_impl.cpp
        cell/scheduler_prb.cpp)

add_library(srsgnb_sched STATIC ${SOURCES} $<TARGET_OBJECTS:common_sched> $<TARGET_OBJECTS:ue_sched>)
target_link_libraries(srsgnb_sched srsgnb_ran rrc_nr_asn1_helpers)

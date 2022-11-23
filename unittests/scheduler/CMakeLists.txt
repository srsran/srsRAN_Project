#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../..)

add_library(scheduler_test_suite scheduler_test_suite.cpp scheduler_output_test_helpers.cpp)
target_link_libraries(scheduler_test_suite srsgnb_sched srslog gtest gtest_main)

add_library(pucch_test_utils pucch_test_utils.cpp)
target_link_libraries(pucch_test_utils srslog mac_configuration_helpers sched_config)

add_executable(pdcch_type0_css_occassions_test support/pdcch/pdcch_type0_css_occassions_test.cpp)
target_link_libraries(pdcch_type0_css_occassions_test srsgnb_sched srslog)
add_test(pdcch_type0_css_occassions_test pdcch_type0_css_occassions_test)

add_executable(pdsch_default_time_allocation_test support/pdsch/pdsch_default_time_allocation_test.cpp)
target_link_libraries(pdsch_default_time_allocation_test srsgnb_sched srslog gtest gtest_main)
add_test(pdsch_default_time_allocation_test pdsch_default_time_allocation_test)

add_executable(pdsch_dmrs_symbol_mask_test support/pdsch/pdsch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pdsch_dmrs_symbol_mask_test srsgnb_sched srslog)
add_test(pdsch_dmrs_symbol_mask_test pdsch_dmrs_symbol_mask_test)

add_executable(pucch_default_resource_test support/pucch/pucch_default_resource_test.cpp)
target_link_libraries(pucch_default_resource_test srsgnb_sched srslog gtest gtest_main)
add_test(pucch_default_resource_test pucch_default_resource_test)

add_executable(pusch_default_time_allocation_test support/pusch/pusch_default_time_allocation_test.cpp)
target_link_libraries(pusch_default_time_allocation_test srsgnb_sched srslog gtest gtest_main)
add_test(pusch_default_time_allocation_test pusch_default_time_allocation_test)

add_executable(pusch_dmrs_symbol_mask_test support/pusch/pusch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pusch_dmrs_symbol_mask_test srsgnb_sched srslog gtest gtest_main)
add_test(pusch_dmrs_symbol_mask_test pusch_dmrs_symbol_mask_test)

add_executable(tbs_calculator_test support/tbs_calculator_test.cpp)
target_link_libraries(tbs_calculator_test srsgnb_sched srslog)
add_test(tbs_calculator_test tbs_calculator_test)

add_executable(prbs_calculator_test support/prbs_calculator_test.cpp)
target_link_libraries(prbs_calculator_test srsgnb_sched srslog gtest gtest_main)
add_test(prbs_calculator_test prbs_calculator_test)

add_executable(dmrs_helpers_test support/dmrs_helpers_test.cpp)
target_link_libraries(dmrs_helpers_test srsgnb_sched srslog)
add_test(dmrs_helpers_test dmrs_helpers_test)

add_executable(sched_prb_test sched_prb_test.cpp)
target_link_libraries(sched_prb_test srsgnb_sched srslog)
add_test(sched_prb_test sched_prb_test)

add_executable(harq_process_test ue_scheduling/harq_process_test.cpp)
target_link_libraries(harq_process_test srsgnb_sched srslog gtest gtest_main)
gtest_discover_tests(harq_process_test)

add_executable(harq_entity_test ue_scheduling/harq_entity_test.cpp)
target_link_libraries(harq_entity_test srsgnb_sched srslog gtest gtest_main)
gtest_discover_tests(harq_entity_test)

add_executable(dl_logical_channel_test ue_scheduling/dl_logical_channel_test.cpp)
target_link_libraries(dl_logical_channel_test srsgnb_sched srslog gtest gtest_main)
add_test(dl_logical_channel_test dl_logical_channel_test)

add_executable(ul_logical_channel_test ul_logical_channel_test.cpp)
target_link_libraries(ul_logical_channel_test srsgnb_sched srslog gtest gtest_main)
add_test(ul_logical_channel_test ul_logical_channel_test)

add_executable(sched_no_ue_test sched_no_ue_test.cpp)
target_link_libraries(sched_no_ue_test srsgnb_sched srslog mac_configuration_helpers
        sched_config
        scheduler_test_suite)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(ssb_scheduler_test ssb_scheduler_test.cpp)
target_link_libraries(ssb_scheduler_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(ssb_scheduler_test ssb_scheduler_test)

add_executable(resource_allocator_test resource_allocator_test.cpp)
target_link_libraries(resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(resource_allocator_test resource_allocator_test)

add_executable(ra_scheduler_test ra_scheduler_test.cpp)
target_link_libraries(ra_scheduler_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(ra_scheduler_test ra_scheduler_test)

add_executable(pdcch_resource_allocator_test pdcch_resource_allocator_test.cpp)
target_link_libraries(pdcch_resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(pdcch_resource_allocator_test pdcch_resource_allocator_test)

add_executable(pucch_allocator_test pucch_allocator_test.cpp)
target_link_libraries(pucch_allocator_test srsgnb_sched scheduler_test_suite pucch_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_allocator_test pucch_allocator_test)

add_executable(pucch_alloc_multi_ue_test pucch_alloc_multi_ue_test.cpp)
target_link_libraries(pucch_alloc_multi_ue_test srsgnb_sched scheduler_test_suite pucch_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_alloc_multi_ue_test pucch_allocator_test)

add_executable(pucch_sr_scheduling_test pucch_sr_scheduling_test.cpp)
target_link_libraries(pucch_sr_scheduling_test srsgnb_sched scheduler_test_suite pucch_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_sr_scheduling_test pucch_sr_scheduling_test)

add_executable(ue_configuration_test ue_configuration_test.cpp)
target_link_libraries(ue_configuration_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        sched_config)
add_test(ue_configuration_test ue_configuration_test)

add_executable(sib1_scheduler_test sib1_scheduler_test.cpp)
target_link_libraries(sib1_scheduler_test srsgnb_sched srslog mac_configuration_helpers sched_config scheduler_test_suite)
add_test(sib1_scheduler_test sib1_scheduler_test)

add_executable(prach_scheduler_test prach_scheduler_test.cpp)
target_link_libraries(prach_scheduler_test srsgnb_sched srslog scheduler_test_suite sched_config gtest gtest_main)
gtest_discover_tests(prach_scheduler_test)

add_executable(srb0_scheduler_test srb0_scheduler_test.cpp)
target_link_libraries(srb0_scheduler_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(srb0_scheduler_test srb0_scheduler_test)

add_executable(scheduler_uci_indication_test scheduler_uci_indication_test.cpp)
target_link_libraries(scheduler_uci_indication_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(scheduler_uci_indication_test scheduler_uci_indication_test)

add_executable(scheduler_retx_test scheduler_retx_test.cpp)
target_link_libraries(scheduler_retx_test srsgnb_sched sched_config scheduler_test_suite gtest gtest_main)
add_test(scheduler_retx_test scheduler_retx_test)

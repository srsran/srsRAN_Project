#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../..)

add_subdirectory(test_utils)
add_subdirectory(support)
add_subdirectory(ue_scheduling)

add_library(uci_test_utils uci_test_utils.cpp)
target_link_libraries(uci_test_utils srslog mac_configuration_helpers sched_config)

add_executable(sched_prb_test cell_resource_grid/sched_prb_test.cpp)
target_link_libraries(sched_prb_test srsgnb_sched srslog)
add_test(sched_prb_test sched_prb_test)

add_executable(sched_no_ue_test scheduler_no_ue_test.cpp)
target_link_libraries(sched_no_ue_test srsgnb_sched srslog mac_configuration_helpers
        sched_config
        scheduler_test_suite)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(resource_allocator_test cell_resource_grid/cell_resource_grid_test.cpp)
target_link_libraries(resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(resource_allocator_test resource_allocator_test)

add_executable(pdcch_resource_allocator_test pdcch/pdcch_resource_allocator_test.cpp)
target_link_libraries(pdcch_resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(pdcch_resource_allocator_test pdcch_resource_allocator_test)

add_executable(pucch_allocator_test pucch_allocator_test.cpp)
target_link_libraries(pucch_allocator_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_allocator_test pucch_allocator_test)

add_executable(pucch_alloc_multi_ue_test pucch_alloc_multi_ue_test.cpp)
target_link_libraries(pucch_alloc_multi_ue_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_alloc_multi_ue_test pucch_allocator_test)

add_executable(uci_sr_scheduling_test uci_sr_scheduling_test.cpp)
target_link_libraries(uci_sr_scheduling_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(uci_sr_scheduling_test uci_sr_scheduling_test)

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

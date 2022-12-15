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
add_subdirectory(uci_and_pucch)
add_subdirectory(common_scheduling)

add_library(uci_test_utils test_utils/uci_test_utils.cpp)
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

add_executable(scheduler_retx_test scheduler_retx_test.cpp)
target_link_libraries(scheduler_retx_test srsgnb_sched sched_config scheduler_test_suite gtest gtest_main)
add_test(scheduler_retx_test scheduler_retx_test)

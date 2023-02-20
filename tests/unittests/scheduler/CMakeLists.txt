#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../../..)

add_subdirectory(test_utils)
add_subdirectory(support)
add_subdirectory(cell_resource_grid)
add_subdirectory(common_scheduling)
add_subdirectory(ue_scheduling)
add_subdirectory(uci_and_pucch)
add_subdirectory(policy)

add_executable(sched_no_ue_test scheduler_no_ue_test.cpp)
target_link_libraries(sched_no_ue_test srsgnb_sched
        scheduler_test_suite
        srslog
        mac_configuration_helpers
        sched_config
        gtest)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(pdcch_resource_allocator_test pdcch/pdcch_resource_allocator_test.cpp)
target_link_libraries(pdcch_resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config gtest gtest_main)
add_test(pdcch_resource_allocator_test pdcch_resource_allocator_test)

add_executable(scheduler_retx_test scheduler_retx_test.cpp)
target_link_libraries(scheduler_retx_test srsgnb_sched sched_config
        scheduler_test_suite
        gtest gtest_main)
add_test(scheduler_retx_test scheduler_retx_test)

add_executable(multiple_ue_sched_test multiple_ue_sched_test.cpp)
target_link_libraries(multiple_ue_sched_test
        srsgnb_sched
        sched_config
        scheduler_test_suite
        mac_configuration_helpers
        gtest
        gtest_main)
gtest_discover_tests(multiple_ue_sched_test)

add_executable(scheduler_metrics_handler_test scheduler_metrics_handler_test.cpp)
target_link_libraries(scheduler_metrics_handler_test
        srsgnb_sched
        sched_config
        gtest
        gtest_main)
gtest_discover_tests(scheduler_metrics_handler_test)
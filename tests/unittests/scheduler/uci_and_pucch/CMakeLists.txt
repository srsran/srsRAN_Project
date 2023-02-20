#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SCHED_TEST_LIBS
        scheduler_test_suite
        uci_test_utils
        sched_config
        mac_configuration_helpers
        gtest)

add_executable(pucch_allocator_test pucch_allocator_test.cpp)
target_link_libraries(pucch_allocator_test srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_allocator_test)

add_executable(pucch_alloc_multi_ue_test pucch_alloc_multi_ue_test.cpp)
target_link_libraries(pucch_alloc_multi_ue_test srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_alloc_multi_ue_test)

add_executable(pucch_res_manager_test pucch_res_manager_test.cpp)
target_link_libraries(pucch_res_manager_test srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_res_manager_test)

add_executable(uci_allocator_test uci_allocator_test.cpp)
target_link_libraries(uci_allocator_test srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(uci_allocator_test)

add_executable(test_pucch_guardbands_sched pucch_guardbands_sched_test.cpp)
target_link_libraries(test_pucch_guardbands_sched srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(test_pucch_guardbands_sched)

add_executable(uci_scheduling_test uci_scheduling_test.cpp)
target_link_libraries(uci_scheduling_test srsgnb_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(uci_scheduling_test)

add_executable(scheduler_uci_indication_test scheduler_uci_indication_test.cpp)
target_link_libraries(scheduler_uci_indication_test
        srsgnb_sched
        scheduler_test_suite
        sched_config
        gtest)
gtest_discover_tests(scheduler_uci_indication_test)

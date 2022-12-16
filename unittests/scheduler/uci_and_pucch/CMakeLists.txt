#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(pucch_allocator_test pucch_allocator_test.cpp)
target_link_libraries(pucch_allocator_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_allocator_test pucch_allocator_test)

add_executable(pucch_alloc_multi_ue_test pucch_alloc_multi_ue_test.cpp)
target_link_libraries(pucch_alloc_multi_ue_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(pucch_alloc_multi_ue_test pucch_allocator_test)

add_executable(uci_allocator_test uci_allocator_test.cpp)
target_link_libraries(uci_allocator_test srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(uci_allocator_test uci_allocator_test)

add_executable(test_pucch_guardbands_sched test_pucch_guardbands_sched.cpp)
target_link_libraries(test_pucch_guardbands_sched srsgnb_sched scheduler_test_suite uci_test_utils srslog mac_configuration_helpers sched_config gtest)
add_test(test_pucch_guardbands_sched test_pucch_guardbands_sched)

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

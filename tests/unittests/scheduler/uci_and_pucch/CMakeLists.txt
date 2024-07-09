#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_executable(uci_pucch_sched_test
        pucch_alloc_common_harq_test.cpp
        pucch_alloc_harq_sr_csi_test.cpp
        pucch_guardbands_sched_test.cpp
        pucch_res_manager_test.cpp
        uci_allocator_test.cpp
        uci_scheduling_test.cpp
        uci_test_utils.cpp
        scheduler_uci_indication_test.cpp)
target_link_libraries(uci_pucch_sched_test
        srsran_sched
        scheduler_test_suite
        srslog
        uci_test_utils
        sched_config
        mac_configuration_helpers
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(uci_pucch_sched_test)

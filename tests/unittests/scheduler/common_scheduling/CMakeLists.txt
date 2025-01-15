#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(common_scheduler_test
        ssb_scheduler_test.cpp
        sib1_scheduler_test.cpp
        prach_scheduler_test.cpp
        ra_scheduler_test.cpp
        paging_scheduler_test.cpp)
target_link_libraries(common_scheduler_test
        srsran_sched
        srslog
        sched_config
        sched_config_manager
        mac_configuration_helpers
        scheduler_test_suite
        sched_test_doubles
        gtest
        gtest_main)
add_test(common_scheduler_test common_scheduler_test)

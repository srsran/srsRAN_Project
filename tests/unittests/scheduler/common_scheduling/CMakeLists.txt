#
# Copyright 2021-2023 Software Radio Systems Limited
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
        mac_configuration_helpers
        scheduler_test_suite
        gtest
        gtest_main)
gtest_discover_tests(common_scheduler_test)

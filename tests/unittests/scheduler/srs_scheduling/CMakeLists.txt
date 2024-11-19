#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srs_sched_test
        srs_scheduler_test.cpp)
target_link_libraries(srs_sched_test
        srsran_sched
        scheduler_test_suite
        srslog
        sched_config
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(srs_sched_test)

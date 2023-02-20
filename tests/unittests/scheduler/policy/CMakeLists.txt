#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(scheduler_policy_test scheduler_policy_test.cpp)
target_link_libraries(scheduler_policy_test srsgnb_sched scheduler_test_suite srslog gtest gtest_main)
gtest_discover_tests(scheduler_policy_test)
#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ssb_scheduler_test ssb_scheduler_test.cpp)
target_link_libraries(ssb_scheduler_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(ssb_scheduler_test ssb_scheduler_test)

add_executable(sib1_scheduler_test sib1_scheduler_test.cpp)
target_link_libraries(sib1_scheduler_test srsgnb_sched srslog mac_configuration_helpers sched_config scheduler_test_suite)
add_test(sib1_scheduler_test sib1_scheduler_test)

add_executable(prach_scheduler_test prach_scheduler_test.cpp)
target_link_libraries(prach_scheduler_test srsgnb_sched srslog scheduler_test_suite sched_config gtest gtest_main)
gtest_discover_tests(prach_scheduler_test)

add_executable(ra_scheduler_test ra_scheduler_test.cpp)
target_link_libraries(ra_scheduler_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(ra_scheduler_test ra_scheduler_test)


#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(slice_scheduler_test slice_scheduler_test.cpp)
target_link_libraries(slice_scheduler_test
        srsran_sched
        scheduler_test_utils
        scheduler_test_suite
        mac_configuration_helpers
        sched_test_doubles
        gtest
        gtest_main
)
add_test(slice_scheduler_test slice_scheduler_test)

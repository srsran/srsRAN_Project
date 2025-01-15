#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(sched_config_test csi_helper_test.cpp)
target_link_libraries(sched_config_test sched_config srslog gtest gtest_main)
add_test(sched_config_test sched_config_test)

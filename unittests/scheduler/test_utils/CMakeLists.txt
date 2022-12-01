#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(scheduler_test_suite scheduler_test_suite.cpp scheduler_output_test_helpers.cpp)
target_link_libraries(scheduler_test_suite srsgnb_sched srslog gtest gtest_main)

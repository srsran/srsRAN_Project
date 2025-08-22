#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "du_high")

include_directories(../../..)

add_executable(du_high_clock_controller_test du_high_clock_controller_test.cpp)
target_link_libraries(du_high_clock_controller_test srsran_du_high srsran_support srslog gtest gtest_main)
add_test(du_high_clock_controller_test du_high_clock_controller_test)

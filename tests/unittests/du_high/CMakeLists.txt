#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "du_high")

add_executable(du_high_test test_utils/du_high_test_bench.cpp du_high_test.cpp)
target_link_libraries(du_high_test srsran_du_high f1ap_du_test_helpers srsran_gateway gtest gtest_main)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(du_high_test)
add_test(du_high_test du_high_test)
set_tests_properties(du_high_test PROPERTIES LABELS "tsan")

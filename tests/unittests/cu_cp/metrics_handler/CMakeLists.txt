#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_cp_metrics_handler_test metrics_handler_test.cpp)
target_include_directories(cu_cp_metrics_handler_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_metrics_handler_test srsran_support srsran_cu_cp srslog gtest gtest_main)

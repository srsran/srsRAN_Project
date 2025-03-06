#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(rate_limiting_test token_bucket_test.cpp)
target_link_libraries(rate_limiting_test srsran_support srslog gtest gtest_main)
target_include_directories(rate_limiting_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(rate_limiting_test rate_limiting_test)

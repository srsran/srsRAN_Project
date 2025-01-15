#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "support")

add_executable(context_repository_helpers_test context_repository_helpers_test.cpp)
target_link_libraries(context_repository_helpers_test srsran_support gtest gtest_main)
target_include_directories(context_repository_helpers_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/ofh/support)
gtest_discover_tests(context_repository_helpers_test)

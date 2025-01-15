#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "psup")

add_executable(psup_packing_test psup_packing_test.cpp)
target_include_directories(psup_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(psup_packing_test srsran_psup srsran_support srslog gtest gtest_main)
gtest_discover_tests(psup_packing_test)

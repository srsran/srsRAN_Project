#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "nrup")

add_executable(nrup_packing_test nrup_packing_test.cpp)
target_include_directories(nrup_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(nrup_packing_test srsran_nrup srsran_support srslog gtest gtest_main)
gtest_discover_tests(nrup_packing_test)

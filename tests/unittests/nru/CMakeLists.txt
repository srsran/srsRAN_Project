#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "nru")

add_executable(nru_packing_test nru_packing_test.cpp)
target_include_directories(nru_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(nru_packing_test srsran_nru srsran_support srslog gtest gtest_main)
gtest_discover_tests(nru_packing_test)

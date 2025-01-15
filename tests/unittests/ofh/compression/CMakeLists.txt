#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

if (USE_PHY_TESTVECTORS)
  add_executable(ofh_compression_test ofh_compression_test.cpp)
  target_link_libraries(ofh_compression_test srsran_ofh_compression srslog gtest gtest_main)
  add_test_vector(ofh_compression_test ofh_compression_test_data.tar.gz "")
  gtest_discover_tests(ofh_compression_test)
endif (USE_PHY_TESTVECTORS)

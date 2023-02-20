#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(short_block_encoder_test short_block_encoder_test.cpp)
target_link_libraries(short_block_encoder_test srsran_channel_coding srslog)
add_test_vector(short_block_encoder_test short_block_encoder_test_data.tar.gz "")

add_executable(short_block_detector_test short_block_detector_test.cpp)
target_link_libraries(short_block_detector_test srsran_channel_coding srsran_upper_phy_support srslog)
add_test_vector(short_block_detector_test short_block_detector_test_data.tar.gz "")

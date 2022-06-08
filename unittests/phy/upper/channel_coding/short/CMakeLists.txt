#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(short_block_encoder_test short_block_encoder_test.cpp)
target_link_libraries(short_block_encoder_test srsgnb_channel_coding srslog)
add_custom_command(
        TARGET short_block_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/short_block_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/short_block_encoder_test_data.tar.gz
        VERBATIM
)
add_test(short_block_encoder_test short_block_encoder_test)

add_executable(short_block_detector_test short_block_detector_test.cpp)
target_link_libraries(short_block_detector_test srsgnb_channel_coding srsgnb_upper_phy_support srslog)
add_custom_command(
        TARGET short_block_detector_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/short_block_detector_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/short_block_detector_test_data.tar.gz
        VERBATIM
)
add_test(short_block_detector_test short_block_detector_test)

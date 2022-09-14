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

add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
target_link_libraries(ldpc_enc_dec_test srsgnb_channel_coding srslog gtest gtest_main)
add_custom_command(
    TARGET ldpc_enc_dec_test
    POST_BUILD
    WORKING_DIRECTORY ${TEST_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_encoder_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_encoder_test_data.tar.gz
    VERBATIM
    )
add_test(ldpc_enc_dec_test ldpc_enc_dec_test)

add_executable(ldpc_rm_test ldpc_rm_test.cpp)
target_link_libraries(ldpc_rm_test srsgnb_channel_coding srslog)
add_custom_command(
    TARGET ldpc_rm_test
    POST_BUILD
    WORKING_DIRECTORY ${TEST_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_rate_matcher_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_rate_matcher_test_data.tar.gz
    VERBATIM
    )
add_test(ldpc_rm_test ldpc_rm_test)

add_executable(ldpc_segmenter_test ldpc_segmenter_test.cpp)
target_link_libraries(ldpc_segmenter_test srsgnb_channel_coding srslog)
add_custom_command(
    TARGET ldpc_segmenter_test
    POST_BUILD
    WORKING_DIRECTORY ${TEST_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_segmenter_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_segmenter_test_data.tar.gz
    VERBATIM
    )
add_test(ldpc_segmenter_test ldpc_segmenter_test)

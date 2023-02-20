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

add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
target_link_libraries(ldpc_enc_dec_test srsgnb_channel_coding srslog gtest gtest_main)
add_test_vector(ldpc_enc_dec_test ldpc_encoder_test_data.tar.gz "")

add_executable(ldpc_rm_test ldpc_rm_test.cpp)
target_link_libraries(ldpc_rm_test srsgnb_channel_coding srslog gtest gtest_main)
add_test_vector(ldpc_rm_test ldpc_rate_matcher_test_data.tar.gz "")

add_executable(ldpc_segmenter_test ldpc_segmenter_test.cpp)
target_link_libraries(ldpc_segmenter_test srsgnb_channel_coding srslog gtest gtest_main)
add_test_vector(ldpc_segmenter_test ldpc_segmenter_test_data.tar.gz "")

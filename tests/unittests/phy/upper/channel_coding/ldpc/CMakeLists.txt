#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
    target_link_libraries(ldpc_enc_dec_test srsran_channel_coding srslog gtest gtest_main)
    add_test_vector(ldpc_enc_dec_test ldpc_encoder_test_data.tar.gz "")

    add_executable(ldpc_rm_test ldpc_rm_test.cpp)
    target_link_libraries(ldpc_rm_test srsran_channel_coding srslog gtest gtest_main)
    add_test_vector(ldpc_rm_test ldpc_rate_matcher_test_data.tar.gz "")

    add_executable(ldpc_segmenter_test ldpc_segmenter_test.cpp)
    target_link_libraries(ldpc_segmenter_test srsran_channel_coding srslog gtest gtest_main)
    add_test_vector(ldpc_segmenter_test ldpc_segmenter_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

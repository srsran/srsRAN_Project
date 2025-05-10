#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(uci_decoder_test uci_decoder_test.cpp)
    target_link_libraries(uci_decoder_test srsran_channel_processors srsran_channel_coding srsran_upper_phy_support gtest gtest_main)
    add_test_vector(uci_decoder_test uci_decoder_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

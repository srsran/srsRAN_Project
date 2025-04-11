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

add_executable(ssb_processor_unittest ssb_processor_unittest.cpp)
target_link_libraries(ssb_processor_unittest srsran_channel_processors srslog)
add_test(ssb_processor_unittest ssb_processor_unittest)

if (USE_PHY_TESTVECTORS)
    add_executable(pbch_encoder_test pbch_encoder_test.cpp)
    target_link_libraries(pbch_encoder_test srsran_channel_processors srslog)
    add_test_vector(pbch_encoder_test pbch_encoder_test_data.tar.gz "")

    add_executable(pbch_modulator_test pbch_modulator_test.cpp)
    target_link_libraries(pbch_modulator_test srsran_channel_processors srslog)
    add_test_vector(pbch_modulator_test pbch_modulator_test_data.tar.gz "")

    add_executable(ssb_processor_vectortest ssb_processor_vectortest.cpp)
    target_link_libraries(ssb_processor_vectortest srsran_channel_processors srslog)
    add_test_vector(ssb_processor_vectortest ssb_processor_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

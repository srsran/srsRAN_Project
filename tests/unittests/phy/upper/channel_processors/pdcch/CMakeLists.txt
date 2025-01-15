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

add_executable(pdcch_processor_unittest pdcch_processor_unittest.cpp)
target_link_libraries(pdcch_processor_unittest srsran_channel_processors srslog)
add_test(pdcch_processor_unittest pdcch_processor_unittest)
target_include_directories(pdcch_processor_unittest PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/upper/signal_processors)

if (USE_PHY_TESTVECTORS)
    add_executable(pdcch_encoder_test pdcch_encoder_test.cpp)
    target_link_libraries(pdcch_encoder_test srsran_channel_processors srslog)
    add_test_vector(pdcch_encoder_test pdcch_encoder_test_data.tar.gz "")

    add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
    target_link_libraries(pdcch_modulator_test
            srsran_channel_processors
            srsran_channel_precoder
            srslog
            srsran_channel_modulation
            srsran_sequence_generators)
    add_test_vector(pdcch_modulator_test pdcch_modulator_test_data.tar.gz "")
    target_include_directories(pdcch_modulator_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support)

    add_executable(pdcch_processor_vectortest pdcch_processor_vectortest.cpp)
    target_link_libraries(pdcch_processor_vectortest srsran_channel_processors srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(pdcch_processor_vectortest pdcch_processor_test_data.tar.gz "")
    target_include_directories(pdcch_processor_vectortest PRIVATE
            ${CMAKE_SOURCE_DIR}/tests/unittests/phy/
            ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support)
endif (USE_PHY_TESTVECTORS)

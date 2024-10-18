#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pusch_processor_validator_test pusch_processor_validator_test.cpp)
target_link_libraries(pusch_processor_validator_test
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
        srsran_transform_precoding
        gtest
        gtest_main)
add_test(pusch_processor_validator_test pusch_processor_validator_test)

add_executable(pusch_processor_unittest pusch_processor_unittest.cpp)
target_link_libraries(pusch_processor_unittest srsran_channel_processors srslog gtest gtest_main)
add_test(pusch_processor_unittest pusch_processor_unittest)

if (USE_PHY_TESTVECTORS)
    add_executable(pusch_decoder_test pusch_decoder_vectortest.cpp)
    set(PUSCH_DECODER_LIBRARIES srsran_channel_processors srslog srsran_upper_phy_support)

    if (ENABLE_PUSCH_HWACC)
        set_source_files_properties(pusch_decoder_vectortest.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PUSCH_ENABLED")
        list(APPEND PUSCH_DECODER_LIBRARIES srsran_hal_pusch)
    endif (ENABLE_PUSCH_HWACC)

    target_link_libraries(pusch_decoder_test ${PUSCH_DECODER_LIBRARIES})
    add_test_vector(pusch_decoder_test pusch_decoder_test_data.tar.gz "-e0 -i2")
    add_test(pusch_decoder_early_stop_test pusch_decoder_test -e1 -i6)

    add_executable(pusch_demodulator_vectortest pusch_demodulator_vectortest.cpp)
    target_link_libraries(pusch_demodulator_vectortest
            srsran_channel_processors
            srsran_channel_equalizer
            srsran_phy_support
            srslog
            srsran_upper_phy_support
            srsran_transform_precoding
            gtest
            gtest_main)
    add_test_vector(pusch_demodulator_vectortest pusch_demodulator_test_data.tar.gz "")

    add_executable(pusch_processor_vectortest pusch_processor_vectortest.cpp)
    set(PUSCH_PROCESSOR_LIBRARIES srsran_channel_equalizer
            srsran_channel_processors
            srsran_phy_support
            srslog
            srsran_upper_phy_support
            srsran_transform_precoding
            gtest
            gtest_main)

    if (ENABLE_PUSCH_HWACC)
        set_source_files_properties(pusch_processor_vectortest.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PUSCH_ENABLED")
        list(APPEND PUSCH_PROCESSOR_LIBRARIES srsran_hal_pusch)
    endif (ENABLE_PUSCH_HWACC)

    target_link_libraries(pusch_processor_vectortest ${PUSCH_PROCESSOR_LIBRARIES})
    add_test_vector(pusch_processor_vectortest pusch_processor_test_data.tar.gz "")

    add_executable(ulsch_demultiplex_test ulsch_demultiplex_test.cpp)
    target_link_libraries(ulsch_demultiplex_test srsran_channel_processors srsran_channel_coding gtest gtest_main)
    add_test_vector(ulsch_demultiplex_test ulsch_demultiplex_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

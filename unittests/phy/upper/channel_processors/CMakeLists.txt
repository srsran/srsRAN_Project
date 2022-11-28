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

add_executable(pbch_encoder_test pbch_encoder_test.cpp)
target_link_libraries(pbch_encoder_test srsgnb_channel_processors srslog)
add_test(pbch_encoder_test pbch_encoder_test)

add_custom_command(
        TARGET pbch_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pbch_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pbch_encoder_test_data.tar.gz
        VERBATIM)

add_executable(pbch_modulator_test pbch_modulator_test.cpp)
target_link_libraries(pbch_modulator_test srsgnb_channel_processors srslog)
add_test(pbch_modulator_test pbch_modulator_test)

add_custom_command(
        TARGET pbch_modulator_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test_data.tar.gz
        VERBATIM)

add_executable(pdcch_encoder_test pdcch_encoder_test.cpp)
target_link_libraries(pdcch_encoder_test srsgnb_channel_processors srslog)
add_test(pdcch_encoder_test pdcch_encoder_test)

add_custom_command(
        TARGET pdcch_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_encoder_test_data.tar.gz
        VERBATIM)

add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
target_link_libraries(pdcch_modulator_test srsgnb_channel_processors srslog srsgnb_channel_modulation srsgnb_sequence_generators)
add_test(pdcch_modulator_test pdcch_modulator_test)

add_custom_command(
        TARGET pdcch_modulator_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_modulator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_modulator_test_data.tar.gz
        VERBATIM)

add_executable(pdcch_processor_unittest pdcch_processor_unittest.cpp)
target_link_libraries(pdcch_processor_unittest srsgnb_channel_processors srslog)
add_test(pdcch_processor_unittest pdcch_processor_unittest)

add_executable(pdsch_encoder_test pdsch_encoder_test.cpp)
target_link_libraries(pdsch_encoder_test srsgnb_channel_processors srslog)
add_test(pdsch_encoder_test pdsch_encoder_test)

add_custom_command(
        TARGET pdsch_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_encoder_test_data.tar.gz
        VERBATIM)

add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
target_link_libraries(pdsch_modulator_test srsgnb_channel_processors srslog)
add_test(pdsch_modulator_test pdsch_modulator_test)

add_custom_command(
        TARGET pdsch_modulator_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_modulator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_modulator_test_data.tar.gz
        VERBATIM)

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest srsgnb_channel_processors srslog)
add_test(pdsch_processor_unittest pdsch_processor_unittest)

add_executable(prach_detector_unittest prach_detector_unittest.cpp)
target_link_libraries(prach_detector_unittest srsgnb_channel_processors srsgnb_generic_funcs srslog gtest gtest_main)
add_test(prach_detector_unittest prach_detector_unittest)

add_executable(prach_detector_vectortest prach_detector_vectortest.cpp)
target_link_libraries(prach_detector_vectortest srsgnb_channel_processors srsgnb_generic_funcs srsgnb_phy_support srslog gtest gtest_main)
add_test(prach_detector_vectortest prach_detector_vectortest)

add_executable(prach_generator_vectortest prach_generator_vectortest.cpp)
target_link_libraries(prach_generator_vectortest srsgnb_channel_processors srsgnb_generic_funcs srslog gtest gtest_main)
add_test(prach_generator_vectortest prach_generator_vectortest)

add_custom_command(
        TARGET prach_generator_vectortest prach_detector_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/prach_generator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/prach_generator_test_data.tar.gz
        VERBATIM)

add_executable(pucch_demodulator_format2_test pucch_demodulator_format2_test.cpp)
target_link_libraries(pucch_demodulator_format2_test
        srsgnb_channel_processors
        srsgnb_channel_equalizer
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pucch_demodulator_format2_test pucch_demodulator_format2_test)

add_custom_command(
        TARGET pucch_demodulator_format2_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pucch_demodulator_format2_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pucch_demodulator_format2_test_data.tar.gz
        VERBATIM)

add_executable(pucch_detector_test pucch_detector_test.cpp)
target_link_libraries(pucch_detector_test srsgnb_channel_processors srslog gtest gtest_main)
add_custom_command(
        TARGET pucch_detector_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pucch_detector_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pucch_detector_test_data.tar.gz
        VERBATIM
)
add_test(pucch_detector_test pucch_detector_test)

add_executable(pucch_processor_format1_unittest pucch_processor_format1_unittest.cpp)
target_link_libraries(pucch_processor_format1_unittest
        srsgnb_channel_equalizer
        srsgnb_channel_processors
        srslog
        gtest
        gtest_main)
add_test(pucch_processor_format1_unittest pucch_processor_format1_unittest)

add_executable(pucch_processor_format1_vectortest pucch_processor_format1_vectortest.cpp)
target_link_libraries(pucch_processor_format1_vectortest
        srsgnb_channel_equalizer
        srsgnb_phy_support
        srsgnb_channel_processors
        srslog
        gtest
        gtest_main)
add_test(pucch_processor_format1_vectortest pucch_processor_format1_vectortest)

add_custom_command(
        TARGET pucch_processor_format1_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pucch_processor_format1_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pucch_processor_format1_test_data.tar.gz
        VERBATIM)

add_executable(pucch_processor_format2_vectortest pucch_processor_format2_vectortest.cpp)
target_link_libraries(pucch_processor_format2_vectortest
        srsgnb_channel_processors
        srsgnb_channel_equalizer
        srsgnb_phy_support
        gtest
        gtest_main)
add_test(pucch_processor_format2_vectortest pucch_processor_format2_vectortest)

add_custom_command(
        TARGET pucch_processor_format2_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pucch_processor_format2_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pucch_processor_format2_test_data.tar.gz
        VERBATIM)

add_executable(pucch_processor_validator_format2_test pucch_processor_validator_format2_test.cpp)
target_link_libraries(pucch_processor_validator_format2_test
        srsgnb_channel_equalizer
        srsgnb_channel_processors
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pucch_processor_validator_format2_test pucch_processor_validator_format2_test)

add_executable(pucch_processor_validator_format1_test pucch_processor_validator_format1_test.cpp)
target_link_libraries(pucch_processor_validator_format1_test
        srsgnb_channel_equalizer
        srsgnb_channel_processors
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pucch_processor_validator_format1_test pucch_processor_validator_format1_test)

add_executable(pusch_decoder_test pusch_decoder_test.cpp)
target_link_libraries(pusch_decoder_test srsgnb_channel_processors srslog srsgnb_upper_phy_support)
add_test(pusch_decoder_test pusch_decoder_test -e0 -i2)
add_test(pusch_decoder_early_stop_test pusch_decoder_test -e1 -i6)

add_custom_command(
        TARGET pusch_decoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pusch_decoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pusch_decoder_test_data.tar.gz
        VERBATIM)

add_executable(pusch_demodulator_vectortest pusch_demodulator_vectortest.cpp)
target_link_libraries(pusch_demodulator_vectortest
        srsgnb_channel_processors
        srsgnb_channel_equalizer
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pusch_demodulator_vectortest pusch_demodulator_vectortest)

add_custom_command(
        TARGET pusch_demodulator_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pusch_demodulator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pusch_demodulator_test_data.tar.gz
        VERBATIM)

add_executable(pusch_processor_validator_test pusch_processor_validator_test.cpp)
target_link_libraries(pusch_processor_validator_test
        srsgnb_channel_equalizer
        srsgnb_channel_processors
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pusch_processor_validator_test pusch_processor_validator_test)

add_executable(pusch_processor_vectortest pusch_processor_vectortest.cpp)
target_link_libraries(pusch_processor_vectortest
        srsgnb_channel_equalizer
        srsgnb_channel_processors
        srsgnb_phy_support
        srslog
        srsgnb_upper_phy_support
        gtest
        gtest_main)
add_test(pusch_processor_vectortest pusch_processor_vectortest)

add_custom_command(
        TARGET pusch_processor_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pusch_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pusch_processor_test_data.tar.gz
        VERBATIM)

add_executable(pusch_processor_unittest pusch_processor_unittest.cpp)
target_link_libraries(pusch_processor_unittest srsgnb_channel_processors srslog gtest gtest_main)
add_test(pusch_processor_unittest pusch_processor_unittest)

add_executable(ssb_processor_unittest ssb_processor_unittest.cpp)
target_link_libraries(ssb_processor_unittest srsgnb_channel_processors srslog)
add_test(ssb_processor_unittest ssb_processor_unittest)

add_executable(ssb_processor_vectortest ssb_processor_vectortest.cpp)
target_link_libraries(ssb_processor_vectortest srsgnb_channel_processors srsgnb_lower_phy_modulation srslog)
add_test(ssb_processor_vectortest ssb_processor_vectortest)

add_custom_command(
        TARGET ssb_processor_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ssb_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ssb_processor_test_data.tar.gz
        VERBATIM)

add_executable(uci_decoder_test uci_decoder_test.cpp)
target_link_libraries(uci_decoder_test srsgnb_channel_processors srsgnb_channel_coding srsgnb_upper_phy_support)
add_custom_command(
        TARGET uci_decoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/uci_decoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/uci_decoder_test_data.tar.gz
        VERBATIM
)
add_test(uci_decoder_test uci_decoder_test)

add_executable(ulsch_demultiplex_test ulsch_demultiplex_test.cpp)
target_link_libraries(ulsch_demultiplex_test srsgnb_channel_processors srsgnb_channel_coding gtest gtest_main)
add_custom_command(
        TARGET ulsch_demultiplex_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ulsch_demultiplex_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ulsch_demultiplex_test_data.tar.gz
        VERBATIM
)
add_test(ulsch_demultiplex_test ulsch_demultiplex_test)

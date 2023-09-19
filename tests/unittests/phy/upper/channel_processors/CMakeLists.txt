#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_executable(pdcch_processor_unittest pdcch_processor_unittest.cpp)
target_link_libraries(pdcch_processor_unittest srsran_channel_processors srslog)
add_test(pdcch_processor_unittest pdcch_processor_unittest)

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest srsran_channel_processors srslog gtest gtest_main)
add_test(pdsch_processor_unittest pdsch_processor_unittest)

add_executable(pdsch_processor_validator_test pdsch_processor_validator_test.cpp)
target_link_libraries(pdsch_processor_validator_test
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
       
        gtest
        gtest_main)
add_test(pdsch_processor_validator_test pdsch_processor_validator_test)

add_executable(pucch_processor_format1_unittest pucch_processor_format1_unittest.cpp)
target_link_libraries(pucch_processor_format1_unittest
        srsran_channel_equalizer
        srsran_channel_processors
        srslog
        gtest
        gtest_main)
add_test(pucch_processor_format1_unittest pucch_processor_format1_unittest)

add_executable(pucch_processor_validator_format2_test pucch_processor_validator_format2_test.cpp)
target_link_libraries(pucch_processor_validator_format2_test
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
        gtest
        gtest_main)
add_test(pucch_processor_validator_format2_test pucch_processor_validator_format2_test)

add_executable(pucch_processor_validator_format1_test pucch_processor_validator_format1_test.cpp)
target_link_libraries(pucch_processor_validator_format1_test
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
        gtest
        gtest_main)
add_test(pucch_processor_validator_format1_test pucch_processor_validator_format1_test)

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

    add_executable(pdcch_encoder_test pdcch_encoder_test.cpp)
    target_link_libraries(pdcch_encoder_test srsran_channel_processors srslog)
    add_test_vector(pdcch_encoder_test pdcch_encoder_test_data.tar.gz "")

    add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
    target_link_libraries(pdcch_modulator_test
            srsran_channel_processors
            srslog
            srsran_channel_modulation
            srsran_sequence_generators)
    add_test_vector(pdcch_modulator_test pdcch_modulator_test_data.tar.gz "")

    add_executable(pdcch_processor_vectortest pdcch_processor_vectortest.cpp)
    target_link_libraries(pdcch_processor_vectortest srsran_channel_processors srslog gtest gtest_main)
    add_test_vector(pdcch_processor_vectortest pdcch_processor_test_data.tar.gz "")

    add_executable(pdsch_encoder_test pdsch_encoder_test.cpp)
    target_link_libraries(pdsch_encoder_test srsran_channel_processors srslog)
    add_test_vector(pdsch_encoder_test pdsch_encoder_test_data.tar.gz "")

    add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
    target_link_libraries(pdsch_modulator_test srsran_channel_processors srslog)
    add_test_vector(pdsch_modulator_test pdsch_modulator_test_data.tar.gz "")

    add_executable(pdsch_processor_vectortest pdsch_processor_vectortest.cpp)
    target_link_libraries(pdsch_processor_vectortest srsran_channel_processors srslog gtest gtest_main)
    add_test_vector(pdsch_processor_vectortest pdsch_processor_test_data.tar.gz "")

    add_executable(prach_detector_vectortest prach_detector_vectortest.cpp)
    target_link_libraries(prach_detector_vectortest srsran_channel_processors srsran_generic_funcs srsran_phy_support srslog gtest gtest_main)
    add_test_vector(prach_detector_vectortest prach_detector_test_data.tar.gz "")

    add_executable(prach_generator_vectortest prach_generator_vectortest.cpp)
    target_link_libraries(prach_generator_vectortest srsran_channel_processors srsran_generic_funcs srslog gtest gtest_main)
    add_test_vector(prach_generator_vectortest prach_generator_test_data.tar.gz "")

    add_executable(pucch_demodulator_format2_test pucch_demodulator_format2_test.cpp)
    target_link_libraries(pucch_demodulator_format2_test
            srsran_channel_processors
            srsran_channel_equalizer
            srsran_phy_support
            srslog
            srsran_upper_phy_support
            gtest
            gtest_main)
    add_test_vector(pucch_demodulator_format2_test pucch_demodulator_format2_test_data.tar.gz "")

    add_executable(pucch_detector_test pucch_detector_test.cpp)
    target_link_libraries(pucch_detector_test srsran_channel_processors srsran_channel_equalizer srslog gtest gtest_main)
    add_test_vector(pucch_detector_test pucch_detector_test_data.tar.gz "")

    add_executable(pucch_processor_format1_vectortest pucch_processor_format1_vectortest.cpp)
    target_link_libraries(pucch_processor_format1_vectortest
            srsran_channel_equalizer
            srsran_phy_support
            srsran_channel_processors
            srslog
            gtest
            gtest_main)
    add_test_vector(pucch_processor_format1_vectortest pucch_processor_format1_test_data.tar.gz "")

    add_executable(pucch_processor_format2_vectortest pucch_processor_format2_vectortest.cpp)
    target_link_libraries(pucch_processor_format2_vectortest
            srsran_channel_processors
            srsran_channel_equalizer
            srsran_phy_support
            gtest
            gtest_main)
    add_test(pucch_processor_format2_vectortest pucch_processor_format2_vectortest)
    add_test_vector(pucch_processor_format2_vectortest pucch_processor_format2_test_data.tar.gz "")

    add_executable(ssb_processor_vectortest ssb_processor_vectortest.cpp)
    target_link_libraries(ssb_processor_vectortest srsran_channel_processors srsran_lower_phy_modulation srslog)
    add_test_vector(ssb_processor_vectortest ssb_processor_test_data.tar.gz "")

    add_executable(uci_decoder_test uci_decoder_test.cpp)
    target_link_libraries(uci_decoder_test srsran_channel_processors srsran_channel_coding srsran_upper_phy_support)
    add_test_vector(uci_decoder_test uci_decoder_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

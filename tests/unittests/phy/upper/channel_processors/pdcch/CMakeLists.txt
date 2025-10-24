#
# Copyright 2021-2025 Software Radio Systems Limited
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
target_include_directories(pdcch_processor_unittest PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support
        ${CMAKE_SOURCE_DIR}/tests/unittests/phy/upper/signal_processors/pdcch)

if (USE_PHY_TESTVECTORS)
    add_executable(pdcch_encoder_test pdcch_encoder_test.cpp)
    target_link_libraries(pdcch_encoder_test srsran_channel_processors srslog)
    add_test_vector(pdcch_encoder_test pdcch_encoder_test_data.tar.gz "")

    add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
    target_link_libraries(pdcch_modulator_test
            srsran_channel_coding
            srsran_channel_modulation
            srsran_channel_precoder
            srsran_channel_processors
            srsran_sequence_generators
            srsran_signal_processors
            srslog)
    add_test_vector(pdcch_modulator_test pdcch_modulator_test_data.tar.gz "")
    target_include_directories(pdcch_modulator_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support)

    add_executable(pdcch_processor_vectortest pdcch_processor_vectortest.cpp)
    target_link_libraries(pdcch_processor_vectortest srsran_channel_processors srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(pdcch_processor_vectortest pdcch_processor_test_data.tar.gz "")
    target_include_directories(pdcch_processor_vectortest PRIVATE
            ${CMAKE_SOURCE_DIR}/tests/unittests/phy/
            ${CMAKE_SOURCE_DIR}/tests/unittests/phy/support)
endif (USE_PHY_TESTVECTORS)

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

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest srsran_channel_processors srslog gtest gtest_main)
add_test(pdsch_processor_unittest pdsch_processor_unittest)

add_executable(pdsch_processor_validator_test pdsch_processor_validator_test.cpp)
target_link_libraries(pdsch_processor_validator_test
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_channel_processors
        srsran_phy_support
        srslog
        srsran_upper_phy_support
        gtest
        gtest_main)
add_test(pdsch_processor_validator_test pdsch_processor_validator_test)

if (USE_PHY_TESTVECTORS)
    add_executable(pdsch_encoder_test pdsch_encoder_test.cpp)
    set(PDSCH_ENCODER_LIBRARIES srsran_channel_processors srslog)

    if (ENABLE_PDSCH_HWACC)
        set_source_files_properties(pdsch_encoder_test.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
        list(APPEND PDSCH_ENCODER_LIBRARIES srsran_hal_pdsch)
    endif (ENABLE_PDSCH_HWACC)

    target_link_libraries(pdsch_encoder_test ${PDSCH_ENCODER_LIBRARIES})
    add_test_vector(pdsch_encoder_test pdsch_encoder_test_data.tar.gz "")

    add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
    target_link_libraries(pdsch_modulator_test srsran_channel_processors srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(pdsch_modulator_test pdsch_modulator_test_data.tar.gz "")

    add_executable(pdsch_processor_vectortest pdsch_processor_vectortest.cpp)
    set(PDSCH_PROCESSOR_LIBRARIES srsran_channel_processors srsran_channel_precoder srslog gtest gtest_main)

    if (ENABLE_PDSCH_HWACC)
        set_source_files_properties(pdsch_processor_vectortest.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
        list(APPEND PDSCH_PROCESSOR_LIBRARIES srsran_hal_pdsch)
    endif (ENABLE_PDSCH_HWACC)

    target_link_libraries(pdsch_processor_vectortest ${PDSCH_PROCESSOR_LIBRARIES})
    add_test_vector(pdsch_processor_vectortest pdsch_processor_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

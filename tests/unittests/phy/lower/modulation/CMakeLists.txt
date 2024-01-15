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

add_executable(ofdm_modulator_unittest ofdm_modulator_unittest.cpp)
target_link_libraries(ofdm_modulator_unittest srslog srsran_lower_phy_modulation)
add_test(ofdm_modulator_unittest ofdm_modulator_unittest)

add_executable(ofdm_demodulator_unittest ofdm_demodulator_unittest.cpp)
target_link_libraries(ofdm_demodulator_unittest srslog srsran_lower_phy_modulation)
add_test(ofdm_demodulator_unittest ofdm_demodulator_unittest)

if (USE_PHY_TESTVECTORS)
    add_executable(ofdm_modulator_vectortest ofdm_modulator_vectortest.cpp)
    target_link_libraries(ofdm_modulator_vectortest srslog srsran_lower_phy_modulation srsran_generic_funcs)
    add_test_vector(ofdm_modulator_vectortest ofdm_modulator_test_data.tar.gz "")

    add_executable(ofdm_demodulator_vectortest ofdm_demodulator_vectortest.cpp)
    target_link_libraries(ofdm_demodulator_vectortest srslog srsran_lower_phy_modulation srsran_generic_funcs)
    add_test_vector(ofdm_demodulator_vectortest ofdm_demodulator_test_data.tar.gz "")

    add_executable(ofdm_prach_demodulator_test ofdm_prach_demodulator_test.cpp)
    target_link_libraries(ofdm_prach_demodulator_test srslog
            srsran_lower_phy_modulation
            srsran_generic_funcs
            srsran_phy_support
            gtest
            gtest_main)
    add_test_vector(ofdm_prach_demodulator_test ofdm_prach_demodulator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

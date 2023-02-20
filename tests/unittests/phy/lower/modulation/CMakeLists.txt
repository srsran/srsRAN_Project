#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(ofdm_modulator_unittest ofdm_modulator_unittest.cpp)
target_link_libraries(ofdm_modulator_unittest srslog srsran_lower_phy_modulation)
add_test(ofdm_modulator_unittest ofdm_modulator_unittest)

add_executable(ofdm_modulator_vectortest ofdm_modulator_vectortest.cpp)
target_link_libraries(ofdm_modulator_vectortest srslog srsran_lower_phy_modulation srsran_generic_funcs)
add_test_vector(ofdm_modulator_vectortest ofdm_modulator_test_data.tar.gz "")

add_executable(ofdm_demodulator_unittest ofdm_demodulator_unittest.cpp)
target_link_libraries(ofdm_demodulator_unittest srslog srsran_lower_phy_modulation)
add_test(ofdm_demodulator_unittest ofdm_demodulator_unittest)

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
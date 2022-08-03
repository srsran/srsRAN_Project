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

add_executable(ofdm_modulator_unittest ofdm_modulator_unittest.cpp)
target_link_libraries(ofdm_modulator_unittest srslog srsgnb_lower_phy_modulation)
add_test(ofdm_modulator_unittest ofdm_modulator_unittest)

add_executable(ofdm_modulator_vectortest ofdm_modulator_vectortest.cpp)
target_link_libraries(ofdm_modulator_vectortest srslog srsgnb_lower_phy_modulation srsgnb_generic_funcs)
add_test(ofdm_modulator_vectortest ofdm_modulator_vectortest)

add_custom_command(
  TARGET ofdm_modulator_vectortest
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test_data.tar.gz
  VERBATIM)

add_executable(ofdm_demodulator_unittest ofdm_demodulator_unittest.cpp)
target_link_libraries(ofdm_demodulator_unittest srslog srsgnb_lower_phy_modulation)
add_test(ofdm_demodulator_unittest ofdm_demodulator_unittest)

add_executable(ofdm_demodulator_vectortest ofdm_demodulator_vectortest.cpp)
target_link_libraries(ofdm_demodulator_vectortest srslog srsgnb_lower_phy_modulation srsgnb_generic_funcs)
add_test(ofdm_demodulator_vectortest ofdm_demodulator_vectortest)

add_custom_command(
  TARGET ofdm_demodulator_vectortest
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_demodulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_demodulator_test_data.tar.gz
  VERBATIM)

add_executable(ofdm_prach_demodulator_test ofdm_prach_demodulator_test.cpp)
target_link_libraries(ofdm_prach_demodulator_test srslog
        srsgnb_lower_phy_modulation
        srsgnb_generic_funcs
        srsgnb_phy_support
        gtest
        gtest_main)
add_test(ofdm_prach_demodulator_test ofdm_prach_demodulator_test)


add_custom_command(
        TARGET ofdm_prach_demodulator_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_prach_demodulator_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_prach_demodulator_test_data.tar.gz
        VERBATIM)
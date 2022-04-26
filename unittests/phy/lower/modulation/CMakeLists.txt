file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(ofdm_modulator_unittest ofdm_modulator_unittest.cpp)
target_link_libraries(ofdm_modulator_unittest srslog srsgnb_lower_phy_modulation)
add_test(ofdm_modulator_unittest ofdm_modulator_unittest)

add_executable(ofdm_modulator_vectortest ofdm_modulator_vectortest.cpp)
target_link_libraries(ofdm_modulator_vectortest srslog srsgnb_lower_phy_modulation srsgnb_generic_funcs_dft)
add_test(ofdm_modulator_vectortest ofdm_modulator_vectortest)

add_custom_command(
  TARGET ofdm_modulator_vectortest
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test_data.tar.gz
  VERBATIM)

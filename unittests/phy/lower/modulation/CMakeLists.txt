
set_directory_properties(PROPERTIES LABELS "phy")

add_executable(ofdm_modulator_test ofdm_modulator_test.cpp)
target_link_libraries(ofdm_modulator_test srslog srsgnb_lower_phy_modulation)
add_test(ofdm_modulator_test ofdm_modulator_test)

add_custom_command(
  TARGET ofdm_modulator_test
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/lower/modulation/
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test/data/ofdm_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ofdm_modulator_test/data/ofdm_modulator_test_data.tar.gz
  VERBATIM)

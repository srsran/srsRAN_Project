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
target_link_libraries(pbch_encoder_test pbch_encoder srslog)
add_test(pbch_encoder_test pbch_encoder_test)

add_custom_command(
  TARGET pbch_encoder_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pbch_encoder_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pbch_encoder_test_data.tar.gz
  VERBATIM)

add_executable(pbch_modulator_test pbch_modulator_test.cpp)
target_link_libraries(pbch_modulator_test pbch_modulator srslog)
add_test(pbch_modulator_test pbch_modulator_test)

add_custom_command(
  TARGET pbch_modulator_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pbch_modulator_test_data.tar.gz
  VERBATIM)

add_executable(pdcch_encoder_test pdcch_encoder_test.cpp)
target_link_libraries(pdcch_encoder_test pdcch_encoder srslog)
add_test(pdcch_encoder_test pdcch_encoder_test)

add_custom_command(
        TARGET pdcch_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_encoder_test_data.tar.gz
        VERBATIM)

add_executable(pdcch_modulator_test pdcch_modulator_test.cpp)
target_link_libraries(pdcch_modulator_test pdcch_modulator srslog modulation_mapper sequence_generators)
add_test(pdcch_modulator_test pdcch_modulator_test)

add_custom_command(
    TARGET pdcch_modulator_test
    POST_BUILD
    WORKING_DIRECTORY ${TEST_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_modulator_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdcch_modulator_test_data.tar.gz
    VERBATIM)

add_executable(pdcch_processor_unittest pdcch_processor_unittest.cpp)
target_link_libraries(pdcch_processor_unittest pdcch_processor srslog)
add_test(pdcch_processor_unittest pdcch_processor_unittest)

add_executable(pdsch_encoder_test pdsch_encoder_test.cpp)
target_link_libraries(pdsch_encoder_test srsgnb_pdsch_encoder srslog)

add_custom_command(
        TARGET pdsch_encoder_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_encoder_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_encoder_test_data.tar.gz
        VERBATIM)

add_executable(pdsch_modulator_test pdsch_modulator_test.cpp)
target_link_libraries(pdsch_modulator_test srsgnb_pdsch_modulator srslog)
add_test(pdsch_modulator_test pdsch_modulator_test)

add_custom_command(
  TARGET pdsch_modulator_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_modulator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pdsch_modulator_test_data.tar.gz
  VERBATIM)

add_executable(pdsch_processor_unittest pdsch_processor_unittest.cpp)
target_link_libraries(pdsch_processor_unittest srsgnb_pdsch_processor srslog)

add_executable(ssb_processor_unittest ssb_processor_unittest.cpp)
target_link_libraries(ssb_processor_unittest ssb_processor srslog)
add_test(ssb_processor_unittest ssb_processor_unittest)

add_executable(ssb_processor_vectortest ssb_processor_vectortest.cpp)
target_link_libraries(ssb_processor_vectortest ssb_processor srsgnb_lower_phy_modulation srslog)
add_test(ssb_processor_vectortest ssb_processor_vectortest)

add_custom_command(
        TARGET ssb_processor_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ssb_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ssb_processor_test_data.tar.gz
        VERBATIM)

add_executable(pusch_decoder_test pusch_decoder_test.cpp)
target_link_libraries(pusch_decoder_test pusch_decoder srslog srsgnb_upper_phy_support)

add_custom_command(
  TARGET pusch_decoder_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/pusch_decoder_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pusch_decoder_test_data.tar.gz
  VERBATIM)

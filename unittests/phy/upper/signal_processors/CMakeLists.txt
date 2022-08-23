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

add_executable(dmrs_pbch_processor_test dmrs_pbch_processor_test.cpp)
target_link_libraries(dmrs_pbch_processor_test  srsgnb_signal_processors srsvec srsgnb_sequence_generators srslog)
add_test(dmrs_pbch_processor_test dmrs_pbch_processor_test)

add_custom_command(
  TARGET dmrs_pbch_processor_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pbch_processor_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pbch_processor_test_data.tar.gz
  VERBATIM)

add_executable(dmrs_pdcch_processor_test dmrs_pdcch_processor_test.cpp)
target_link_libraries(dmrs_pdcch_processor_test  srsgnb_signal_processors srsvec srsgnb_sequence_generators srslog)
add_test(dmrs_pdcch_processor_test dmrs_pdcch_processor_test)

add_custom_command(
  TARGET dmrs_pdcch_processor_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pdcch_processor_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pdcch_processor_test_data.tar.gz
  VERBATIM)

add_executable(dmrs_pdsch_processor_test dmrs_pdsch_processor_test.cpp)
target_link_libraries(dmrs_pdsch_processor_test  srsgnb_signal_processors srsvec srsgnb_sequence_generators srslog)
add_test(dmrs_pdsch_processor_test dmrs_pdsch_processor_test)

add_custom_command(
  TARGET dmrs_pdsch_processor_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pdsch_processor_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pdsch_processor_test_data.tar.gz
  VERBATIM)

add_executable(dmrs_pucch_processor_test dmrs_pucch_processor_test.cpp)
target_link_libraries(dmrs_pucch_processor_test  srsgnb_signal_processors srsvec srsgnb_sequence_generators srslog srsgnb_support)
add_test(dmrs_pucch_processor_test dmrs_pucch_processor_test)

add_custom_command(
        TARGET dmrs_pucch_processor_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pucch_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pucch_processor_test_data.tar.gz
        VERBATIM)

add_executable(pss_processor_test pss_processor_test.cpp)
target_link_libraries(pss_processor_test  srsgnb_signal_processors srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test sss_processor_test.cpp)
target_link_libraries(sss_processor_test  srsgnb_signal_processors srsvec srslog)
add_test(sss_processor_test sss_processor_test)

add_executable(csi_rs_processor_test csi_rs_processor_test.cpp)
target_link_libraries(csi_rs_processor_test  srsgnb_signal_processors srsvec srsgnb_sequence_generators srslog srsgnb_upper_phy_support)
add_test(csi_rs_processor_test csi_rs_processor_test)

add_custom_command(
        TARGET csi_rs_processor_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/csi_rs_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/csi_rs_processor_test_data.tar.gz
        VERBATIM)

add_executable(dmrs_pusch_estimator_test dmrs_pusch_estimator_test.cpp)
target_link_libraries(dmrs_pusch_estimator_test  srsgnb_signal_processors srsgnb_sequence_generators srsgnb_phy_support srsvec srslog gtest gtest_main)
add_test(dmrs_pusch_estimator_test dmrs_pusch_estimator_test)

add_custom_command(
  TARGET dmrs_pusch_estimator_test
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pusch_estimator_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dmrs_pusch_estimator_test_data.tar.gz
  VERBATIM)
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

add_executable(pss_processor_test pss_processor_test.cpp)
target_link_libraries(pss_processor_test  srsran_signal_processors srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test sss_processor_test.cpp)
target_link_libraries(sss_processor_test  srsran_signal_processors srsvec srslog)
add_test(sss_processor_test sss_processor_test)

if (USE_PHY_TESTVECTORS)
    add_executable(dmrs_pbch_processor_test dmrs_pbch_processor_test.cpp)
    target_link_libraries(dmrs_pbch_processor_test  srsran_signal_processors srsvec srsran_sequence_generators srslog)
    add_test_vector(dmrs_pbch_processor_test dmrs_pbch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdcch_processor_test dmrs_pdcch_processor_test.cpp)
    target_link_libraries(dmrs_pdcch_processor_test  srsran_signal_processors srsvec srsran_sequence_generators srslog)
    add_test(dmrs_pdcch_processor_test dmrs_pdcch_processor_test)
    add_test_vector(dmrs_pdcch_processor_test dmrs_pdcch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdsch_processor_test dmrs_pdsch_processor_test.cpp)
    target_link_libraries(dmrs_pdsch_processor_test  srsran_signal_processors srsvec srsran_sequence_generators srslog)
    add_test_vector(dmrs_pdsch_processor_test dmrs_pdsch_processor_test_data.tar.gz "")

    add_executable(dmrs_pucch_processor_test dmrs_pucch_processor_test.cpp)
    target_link_libraries(dmrs_pucch_processor_test  srsran_signal_processors srsvec srsran_sequence_generators srslog srsran_support)
    add_test_vector(dmrs_pucch_processor_test dmrs_pucch_processor_test_data.tar.gz "")

    add_executable(dmrs_pusch_estimator_test dmrs_pusch_estimator_test.cpp)
    target_link_libraries(dmrs_pusch_estimator_test  srsran_signal_processors srsran_sequence_generators srsran_phy_support srsvec srslog gtest gtest_main)
    add_test_vector(dmrs_pusch_estimator_test dmrs_pusch_estimator_test_data.tar.gz "")

    add_executable(nzp_csi_rs_generator_test nzp_csi_rs_generator_test.cpp)
    target_link_libraries(nzp_csi_rs_generator_test srsran_signal_processors srsvec srsran_sequence_generators srslog
      srsran_upper_phy_support gtest gtest_main)
    add_test(nzp_csi_rs_generator_test nzp_csi_rs_generator_test)
    add_test_vector(nzp_csi_rs_generator_test nzp_csi_rs_generator_test_data.tar.gz "")

    add_executable(port_channel_estimator_test port_channel_estimator_test.cpp)
    target_link_libraries(port_channel_estimator_test srsran_signal_processors srslog gtest gtest_main)
    add_test_vector(port_channel_estimator_test port_channel_estimator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

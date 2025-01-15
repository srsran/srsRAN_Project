#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pss_processor_test pss_processor_test.cpp)
target_link_libraries(pss_processor_test srsran_signal_processors srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test sss_processor_test.cpp)
target_link_libraries(sss_processor_test srsran_signal_processors srsvec srslog)
add_test(sss_processor_test sss_processor_test)

add_executable(prs_generator_validator_test prs/prs_generator_validator_test.cpp)
target_link_libraries(prs_generator_validator_test srsran_signal_processors srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
add_test(prs_generator_validator_test prs_generator_validator_test)

if (USE_PHY_TESTVECTORS)
    add_executable(prs_generator_vectortest prs/prs_generator_vectortest.cpp)
    target_link_libraries(prs_generator_vectortest srsran_signal_processors srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(prs_generator_vectortest prs/prs_generator_test_data.tar.gz "")

    add_executable(ptrs_pdsch_generator_vectortest ptrs/ptrs_pdsch_generator_vectortest.cpp)
    target_link_libraries(ptrs_pdsch_generator_vectortest srsran_signal_processors srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(ptrs_pdsch_generator_vectortest ptrs/ptrs_pdsch_generator_test_data.tar.gz "")

    add_executable(srs_estimator_test
            srs/srs_estimator_logger_test.cpp
            srs/srs_estimator_validator_test.cpp
            srs/srs_estimator_vectortest.cpp)
    target_link_libraries(srs_estimator_test srsran_signal_processors srsvec srsran_sequence_generators srslog gtest gtest_main)
    add_test_vector(srs_estimator_test srs/srs_estimator_test_data.tar.gz "")

    add_executable(dmrs_pbch_processor_test dmrs_pbch_processor_test.cpp)
    target_link_libraries(dmrs_pbch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog)
    add_test_vector(dmrs_pbch_processor_test dmrs_pbch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdcch_processor_test dmrs_pdcch_processor_test.cpp)
    target_link_libraries(dmrs_pdcch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog srsran_channel_precoder)
    add_test(dmrs_pdcch_processor_test dmrs_pdcch_processor_test)
    add_test_vector(dmrs_pdcch_processor_test dmrs_pdcch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdsch_processor_test dmrs_pdsch_processor_test.cpp)
    target_link_libraries(dmrs_pdsch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog srsran_channel_precoder)
    add_test_vector(dmrs_pdsch_processor_test dmrs_pdsch_processor_test_data.tar.gz "")

    add_executable(dmrs_pucch_estimator_test dmrs_pucch_estimator_test.cpp)
    target_link_libraries(dmrs_pucch_estimator_test srsran_signal_processors srsvec srsran_sequence_generators srslog srsran_support gtest gtest_main)
    add_test_vector(dmrs_pucch_estimator_test dmrs_pucch_estimator_test_data.tar.gz "")

    add_executable(dmrs_pusch_estimator_test dmrs_pusch_estimator_test.cpp)
    target_link_libraries(dmrs_pusch_estimator_test
        srsran_signal_processors
        srsran_sequence_generators
        srsran_phy_support
        srsran_pusch_processor
        srsvec
        srslog
        gtest
        gtest_main
    )
    add_test_vector(dmrs_pusch_estimator_test dmrs_pusch_estimator_test_data.tar.gz "")

    add_executable(nzp_csi_rs_generator_test nzp_csi_rs_generator_test.cpp)
    target_link_libraries(nzp_csi_rs_generator_test srsran_signal_processors srsvec srsran_sequence_generators srslog
            srsran_upper_phy_support gtest gtest_main srsran_channel_precoder)
    add_test(nzp_csi_rs_generator_test nzp_csi_rs_generator_test)
    add_test_vector(nzp_csi_rs_generator_test nzp_csi_rs_generator_test_data.tar.gz "")

    add_executable(port_channel_estimator_test port_channel_estimator_test.cpp)
    target_link_libraries(port_channel_estimator_test srsran_signal_processors srsran_pusch_processor srslog gtest gtest_main)
    add_test_vector(port_channel_estimator_test port_channel_estimator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

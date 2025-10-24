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

add_executable(pss_processor_test ssb/pss_processor_test.cpp)
target_link_libraries(pss_processor_test srsran_ssb_generators srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test ssb/sss_processor_test.cpp)
target_link_libraries(sss_processor_test srsran_ssb_generators srsvec srslog)
add_test(sss_processor_test sss_processor_test)

add_executable(prs_generator_validator_test prs/prs_generator_validator_test.cpp)
target_link_libraries(prs_generator_validator_test srsran_prs_generator srsran_ran srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
add_test(prs_generator_validator_test prs_generator_validator_test)

if (USE_PHY_TESTVECTORS)
    add_executable(prs_generator_vectortest prs/prs_generator_vectortest.cpp)
    target_link_libraries(prs_generator_vectortest srsran_prs_generator srsran_ran srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(prs_generator_vectortest prs/prs_generator_test_data.tar.gz "")

    add_executable(ptrs_pdsch_generator_vectortest ptrs/ptrs_pdsch_generator_vectortest.cpp)
    target_link_libraries(ptrs_pdsch_generator_vectortest srsran_ptrs_generator srsran_ran srsran_phy_support
            srsvec srsran_sequence_generators srsran_channel_precoder srslog gtest gtest_main)
    add_test_vector(ptrs_pdsch_generator_vectortest ptrs/ptrs_pdsch_generator_test_data.tar.gz "")

    add_executable(srs_estimator_test
            srs/srs_estimator_logger_test.cpp
            srs/srs_estimator_validator_test.cpp
            srs/srs_estimator_vectortest.cpp)
    target_link_libraries(srs_estimator_test srsran_srs_estimator srsran_dft srsran_phy_support srsvec srsran_sequence_generators srslog gtest gtest_main)
    add_test_vector(srs_estimator_test srs/srs_estimator_test_data.tar.gz "")

    add_executable(dmrs_pbch_processor_test ssb/dmrs_pbch_processor_test.cpp)
    target_link_libraries(dmrs_pbch_processor_test srsran_ssb_generators srsvec srsran_sequence_generators srslog)
    add_test_vector(dmrs_pbch_processor_test ssb/dmrs_pbch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdcch_processor_test pdcch/dmrs_pdcch_processor_test.cpp)
    target_link_libraries(dmrs_pdcch_processor_test srsran_dmrs_pdcch srsran_ran srsran_phy_support srsvec srsran_sequence_generators srslog srsran_channel_precoder)
    add_test(dmrs_pdcch_processor_test dmrs_pdcch_processor_test)
    add_test_vector(dmrs_pdcch_processor_test pdcch/dmrs_pdcch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdsch_processor_test pdsch/dmrs_pdsch_processor_test.cpp)
    target_link_libraries(dmrs_pdsch_processor_test srsran_dmrs_pdsch srsran_ran srsran_phy_support srsvec srsran_sequence_generators srslog srsran_channel_precoder)
    add_test_vector(dmrs_pdsch_processor_test pdsch/dmrs_pdsch_processor_test_data.tar.gz "")

    add_executable(dmrs_pucch_estimator_test pucch/dmrs_pucch_estimator_test.cpp)
    target_link_libraries(dmrs_pucch_estimator_test srsran_dmrs_pucch srsran_channel_estimator srsran_dft srsvec
        srsran_sequence_generators srslog srsran_support gtest gtest_main)
    add_test_vector(dmrs_pucch_estimator_test pucch/dmrs_pucch_estimator_test_data.tar.gz "")

    add_executable(dmrs_pusch_estimator_test pusch/dmrs_pusch_estimator_test.cpp)
    target_link_libraries(dmrs_pusch_estimator_test
        srsran_dmrs_pusch
        srsran_channel_estimator
        srsran_dft
        srsran_sequence_generators
        srsran_phy_support
        srsran_pusch_processor
        srsvec
        srslog
        gtest
        gtest_main
    )
    add_test_vector(dmrs_pusch_estimator_test pusch/dmrs_pusch_estimator_test_data.tar.gz "")

    add_executable(nzp_csi_rs_generator_test nzp_csi_rs/nzp_csi_rs_generator_test.cpp)
    target_link_libraries(nzp_csi_rs_generator_test srsran_nzp_csi_rs_generator srsvec srsran_sequence_generators srslog
            srsran_upper_phy_support srsran_phy_support gtest gtest_main srsran_channel_precoder)
    add_test(nzp_csi_rs_generator_test nzp_csi_rs_generator_test)
    add_test_vector(nzp_csi_rs_generator_test nzp_csi_rs/nzp_csi_rs_generator_test_data.tar.gz "")

    add_executable(port_channel_estimator_test channel_estimator/port_channel_estimator_test.cpp)
    target_link_libraries(port_channel_estimator_test srsran_channel_estimator srsran_pusch_processor srsran_dft srslog gtest gtest_main)
    add_test_vector(port_channel_estimator_test channel_estimator/port_channel_estimator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

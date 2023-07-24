#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_executable(pss_processor_test pss_processor_test.cpp)
target_link_libraries(pss_processor_test srsran_signal_processors srsvec srslog)
add_test(pss_processor_test pss_processor_test)

add_executable(sss_processor_test sss_processor_test.cpp)
target_link_libraries(sss_processor_test srsran_signal_processors srsvec srslog)
add_test(sss_processor_test sss_processor_test)

if (USE_PHY_TESTVECTORS)
    add_executable(dmrs_pbch_processor_test dmrs_pbch_processor_test.cpp)
    target_link_libraries(dmrs_pbch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog)
    add_test_vector(dmrs_pbch_processor_test dmrs_pbch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdcch_processor_test dmrs_pdcch_processor_test.cpp)
    target_link_libraries(dmrs_pdcch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog resource_grid_mapper_test_doubles)
    add_test(dmrs_pdcch_processor_test dmrs_pdcch_processor_test)
    add_test_vector(dmrs_pdcch_processor_test dmrs_pdcch_processor_test_data.tar.gz "")

    add_executable(dmrs_pdsch_processor_test dmrs_pdsch_processor_test.cpp)
    target_link_libraries(dmrs_pdsch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog resource_grid_mapper_test_doubles)
    add_test_vector(dmrs_pdsch_processor_test dmrs_pdsch_processor_test_data.tar.gz "")

    add_executable(dmrs_pucch_processor_test dmrs_pucch_processor_test.cpp)
    target_link_libraries(dmrs_pucch_processor_test srsran_signal_processors srsvec srsran_sequence_generators srslog srsran_support gtest gtest_main)
    add_test_vector(dmrs_pucch_processor_test dmrs_pucch_processor_test_data.tar.gz "")

    add_executable(dmrs_pusch_estimator_test dmrs_pusch_estimator_test.cpp)
    target_link_libraries(dmrs_pusch_estimator_test srsran_signal_processors srsran_sequence_generators srsran_phy_support srsvec srslog gtest gtest_main)
    add_test_vector(dmrs_pusch_estimator_test dmrs_pusch_estimator_test_data.tar.gz "")

    add_executable(nzp_csi_rs_generator_test nzp_csi_rs_generator_test.cpp)
    target_link_libraries(nzp_csi_rs_generator_test srsran_signal_processors srsvec srsran_sequence_generators srslog
            srsran_upper_phy_support gtest gtest_main resource_grid_mapper_test_doubles)
    add_test(nzp_csi_rs_generator_test nzp_csi_rs_generator_test)
    add_test_vector(nzp_csi_rs_generator_test nzp_csi_rs_generator_test_data.tar.gz "")

    add_executable(port_channel_estimator_test port_channel_estimator_test.cpp)
    target_link_libraries(port_channel_estimator_test srsran_signal_processors srslog gtest gtest_main)
    add_test_vector(port_channel_estimator_test port_channel_estimator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

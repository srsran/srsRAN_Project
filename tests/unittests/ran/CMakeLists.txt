#
# Copyright 2021-2024 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "ran")

include_directories(../../..)

add_executable(csi_report_on_pucch_helpers_test csi_report/csi_report_on_pucch_helpers_test.cpp)
target_link_libraries(csi_report_on_pucch_helpers_test srsran_ran gtest gtest_main)
gtest_discover_tests(csi_report_on_pucch_helpers_test)

add_executable(csi_report_on_pucch_validator_test csi_report/csi_report_on_pucch_validator_test.cpp)
target_link_libraries(csi_report_on_pucch_validator_test srsran_ran gtest gtest_main)
gtest_discover_tests(csi_report_on_pucch_validator_test)

add_executable(csi_report_on_pusch_helpers_test csi_report/csi_report_on_pusch_helpers_test.cpp)
target_link_libraries(csi_report_on_pusch_helpers_test srsran_ran gtest gtest_main)
gtest_discover_tests(csi_report_on_pusch_helpers_test)

add_executable(csi_report_on_pusch_validator_test csi_report/csi_report_on_pusch_validator_test.cpp)
target_link_libraries(csi_report_on_pusch_validator_test srsran_ran gtest gtest_main)
gtest_discover_tests(csi_report_on_pusch_validator_test)

add_library(du_test_suite ssb_coreset0_freq_pos_checker.cpp)
target_link_libraries(du_test_suite srsran_ran gtest)

add_executable(test_ssb_coreset0_freq_pos_generator test_ssb_coreset0_freq_pos_generator.cpp)
target_link_libraries(test_ssb_coreset0_freq_pos_generator srsran_support du_test_suite gtest gtest_main)
add_test(test_ssb_coreset0_freq_pos_generator test_ssb_coreset0_freq_pos_generator)

add_executable(dci_packing_test pdcch/dci_packing_test.cpp)
target_link_libraries(dci_packing_test srsran_support srsran_ran gtest gtest_main)
add_test(dci_packing_test dci_packing_test)

add_executable(dci_packing_validator_test pdcch/dci_packing_validator_test.cpp)
target_link_libraries(dci_packing_validator_test srsran_support srsran_ran gtest gtest_main)
add_test(dci_packing_validator_test dci_packing_validator_test)

add_executable(pdcch_candidates_common pdcch/pdcch_candidates_common_test.cpp)
target_link_libraries(pdcch_candidates_common srsran_support srsran_ran gtest gtest_main)
add_test_vector(pdcch_candidates_common none "")

add_executable(pdcch_candidates_ue pdcch/pdcch_candidates_ue_test.cpp)
target_link_libraries(pdcch_candidates_ue srsran_support srsran_ran gtest gtest_main)
add_test_vector(pdcch_candidates_ue none "")

add_executable(pdcch_type0_css_coreset_config_test pdcch/pdcch_type0_css_coreset_config_test.cpp)
target_link_libraries(pdcch_type0_css_coreset_config_test srsran_ran srslog)
add_test(pdcch_type0_css_coreset_config_test pdcch_type0_css_coreset_config_test)

add_executable(pdsch_mcs_table_test pdsch/pdsch_mcs_table_test.cpp)
target_link_libraries(pdsch_mcs_table_test srsran_ran srslog gtest gtest_main)
add_test(pdsch_mcs_table_test pdsch_mcs_table_test)

add_executable(prach_configuration_test prach/prach_configuration_test.cpp)
target_link_libraries(prach_configuration_test srsran_ran srslog gtest gtest_main)
add_test(prach_configuration_test prach_configuration_test)

add_executable(prach_cyclic_shifts_test prach/prach_cyclic_shifts_test.cpp)
target_link_libraries(prach_cyclic_shifts_test srsran_ran srslog gtest gtest_main)
add_test(prach_cyclic_shifts_test prach_cyclic_shifts_test)

add_executable(prach_frequency_mapping_test prach/prach_frequency_mapping_test.cpp)
target_link_libraries(prach_frequency_mapping_test srsran_ran srslog gtest gtest_main)
add_test(prach_frequency_mapping_test prach_frequency_mapping_test)

add_executable(pucch_info_test pucch/pucch_info_test.cpp)
target_link_libraries(pucch_info_test srsran_ran gtest gtest_main)
add_test(pucch_info_test pucch_info_test)

add_executable(pusch_mcs_table_test pusch/pusch_mcs_table_test.cpp)
target_link_libraries(pusch_mcs_table_test srsran_ran srslog gtest gtest_main)
add_test(pusch_mcs_table_test pusch_mcs_table_test)

add_executable(ulsch_info_test pusch/ulsch_info_test.cpp)
target_link_libraries(ulsch_info_test srsran_ran srslog gtest gtest_main)
add_test_vector(ulsch_info_test none "")

add_executable(resource_allocation_frequency_test resource_allocation/resource_allocation_frequency_test.cpp)
target_link_libraries(resource_allocation_frequency_test srsran_support srsran_ran)
add_test(resource_allocation_frequency_test resource_allocation_frequency_test)

add_executable(uci_part2_size_calculator_test uci/uci_part2_size_calculator_test.cpp)
target_link_libraries(uci_part2_size_calculator_test srsran_support srsran_ran srsvec gtest gtest_main)
add_test(uci_part2_size_calculator_test uci_part2_size_calculator_test)

add_executable(slot_point_test slot_point_test.cpp)
target_link_libraries(slot_point_test srsran_support)
add_test(slot_point_test slot_point_test)

add_executable(band_helper_test band_helper_test.cpp)
target_link_libraries(band_helper_test srsran_ran srslog gtest gtest_main)
target_include_directories(band_helper_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(band_helper_test band_helper_test)

add_executable(ssb_gscn_test ssb_gscn_test.cpp)
target_link_libraries(ssb_gscn_test srsran_ran gtest gtest_main)
add_test(ssb_gscn_test ssb_gscn_test)

add_executable(ssb_mapping_test ssb_mapping_test.cpp)
target_link_libraries(ssb_mapping_test srsran_ran srslog)
add_test(ssb_mapping_test ssb_mapping_test)

add_executable(bcd_helpers_test bcd_helpers_test.cpp)
target_link_libraries(bcd_helpers_test srsran_support gtest gtest_main)
gtest_discover_tests(bcd_helpers_test)

add_executable(nr_cgi_helpers_test nr_cgi_helpers_test.cpp)
target_link_libraries(nr_cgi_helpers_test srsran_support gtest gtest_main)
gtest_discover_tests(nr_cgi_helpers_test)

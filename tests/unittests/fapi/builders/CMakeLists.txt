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

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(crc_indication_builder_test crc_indication_test.cpp)
target_link_libraries(crc_indication_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(crc_indication_builder_test)

add_executable(dl_csi_pdu_builder_test dl_csi_pdu_test.cpp)
target_link_libraries(dl_csi_pdu_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(dl_csi_pdu_builder_test)

add_executable(dl_pdcch_builder_test dl_pdcch_pdu_test.cpp)
target_link_libraries(dl_pdcch_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(dl_pdcch_builder_test)

add_executable(dl_pdsch_builder_test dl_pdsch_pdu_test.cpp)
target_link_libraries(dl_pdsch_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(dl_pdsch_builder_test)

add_executable(dl_ssb_builder_test dl_ssb_pdu_test.cpp)
target_link_libraries(dl_ssb_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(dl_ssb_builder_test)

add_executable(dl_tti_request_test dl_tti_request_test.cpp)
target_link_libraries(dl_tti_request_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(dl_tti_request_test)

add_executable(error_indication_builder_test error_indication_test.cpp)
target_link_libraries(error_indication_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(error_indication_builder_test)

add_executable(rach_indicator_builder_test rach_indicator_test.cpp)
target_link_libraries(rach_indicator_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(rach_indicator_builder_test)

add_executable(rx_data_indication_builder_test rx_data_indication_test.cpp)
target_link_libraries(rx_data_indication_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(rx_data_indication_builder_test)

add_executable(slot_indication_builder_test slot_indication_test.cpp)
target_link_libraries(slot_indication_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(slot_indication_builder_test)

add_executable(tx_data_request_builder_test tx_data_request_test.cpp)
target_link_libraries(tx_data_request_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(tx_data_request_builder_test)

add_executable(tx_precoding_and_beamforming_pdu_builder_test tx_precoding_and_beamforming_pdu_test.cpp)
target_link_libraries(tx_precoding_and_beamforming_pdu_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(tx_precoding_and_beamforming_pdu_builder_test)

add_executable(uci_indication_builder_test uci_indication_test.cpp)
target_link_libraries(uci_indication_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(uci_indication_builder_test)

add_executable(ul_dci_request_builder_test ul_dci_request_test.cpp)
target_link_libraries(ul_dci_request_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(ul_dci_request_builder_test)

add_executable(ul_prach_builder_test ul_prach_pdu_test.cpp)
target_link_libraries(ul_prach_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(ul_prach_builder_test)

add_executable(ul_pucch_builder_test ul_pucch_pdu_test.cpp)
target_link_libraries(ul_pucch_builder_test srsran_support gtest gtest_main)
gtest_discover_tests(ul_pucch_builder_test)

add_executable(ul_pusch_builder_test ul_pusch_pdu_test.cpp)
target_link_libraries(ul_pusch_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(ul_pusch_builder_test)

add_executable(ul_tti_request_builder_test ul_tti_request_test.cpp)
target_link_libraries(ul_tti_request_builder_test srsran_support srslog gtest gtest_main)
gtest_discover_tests(ul_tti_request_builder_test)

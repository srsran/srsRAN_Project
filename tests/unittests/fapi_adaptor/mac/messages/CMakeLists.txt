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

add_library(srsran_mac_translator_helpers STATIC helpers.cpp)

add_executable(mac_fapi_pdcch_adaptor_test dl_pdcch_pdu_test.cpp)
target_link_libraries(mac_fapi_pdcch_adaptor_test srsran_mac_fapi_adaptors srsran_support srslog srsran_mac_translator_helpers gtest gtest_main)
gtest_discover_tests(mac_fapi_pdcch_adaptor_test)

add_executable(mac_fapi_pdsch_adaptor_test dl_pdsch_pdu_test.cpp)
target_link_libraries(mac_fapi_pdsch_adaptor_test srsran_mac_fapi_adaptors srsran_fapi_precoding_matrix_tools srsran_support srslog srsran_mac_translator_helpers gtest gtest_main)
gtest_discover_tests(mac_fapi_pdsch_adaptor_test)

add_executable(mac_fapi_ssb_adaptor_test dl_ssb_pdu_test.cpp)
target_link_libraries(mac_fapi_ssb_adaptor_test srsran_mac_fapi_adaptors srsran_support srslog srsran_mac_translator_helpers gtest gtest_main)
gtest_discover_tests(mac_fapi_ssb_adaptor_test)

add_executable(mac_fapi_prach_adaptor_test ul_prach_pdu_test.cpp)
target_link_libraries(mac_fapi_prach_adaptor_test srsran_mac_fapi_adaptors srsran_support gtest gtest_main srsran_mac_translator_helpers)
gtest_discover_tests(mac_fapi_prach_adaptor_test)

add_executable(mac_fapi_pucch_adaptor_test ul_pucch_pdu_test.cpp)
target_link_libraries(mac_fapi_pucch_adaptor_test srsran_mac_fapi_adaptors srsran_support gtest gtest_main srsran_mac_translator_helpers)
gtest_discover_tests(mac_fapi_pucch_adaptor_test)

add_executable(mac_fapi_pusch_adaptor_test ul_pusch_pdu_test.cpp)
target_link_libraries(mac_fapi_pusch_adaptor_test srsran_mac_fapi_adaptors srsran_support srsran_fapi_uci_part2_tools gtest gtest_main srsran_mac_translator_helpers)
gtest_discover_tests(mac_fapi_pusch_adaptor_test)

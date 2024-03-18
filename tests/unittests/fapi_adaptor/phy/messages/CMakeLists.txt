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

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_executable(fapi_phy_ssb_adaptor_test dl_ssb_pdu_test.cpp)
target_link_libraries(fapi_phy_ssb_adaptor_test srsran_fapi_phy_message_adaptors srslog gtest gtest_main)
gtest_discover_tests(fapi_phy_ssb_adaptor_test)

add_executable(fapi_phy_pdcch_adaptor_test dl_pdcch_pdu_test.cpp)
target_link_libraries(fapi_phy_pdcch_adaptor_test srsran_fapi_phy_message_adaptors srsran_fapi_precoding_matrix_tools srsran_ran srslog gtest gtest_main)
gtest_discover_tests(fapi_phy_pdcch_adaptor_test)

add_executable(fapi_phy_pdsch_adaptor_test dl_pdsch_pdu_test.cpp)
target_link_libraries(fapi_phy_pdsch_adaptor_test srsran_fapi_phy_message_adaptors srsran_upper_phy_support srslog gtest gtest_main)
gtest_discover_tests(fapi_phy_pdsch_adaptor_test)

add_executable(fapi_phy_prach_adaptor_test ul_prach_pdu_test.cpp)
target_link_libraries(fapi_phy_prach_adaptor_test srsran_fapi_phy_message_adaptors gtest gtest_main srsran_fapi_message_builder_test_helpers srslog)
gtest_discover_tests(fapi_phy_prach_adaptor_test)

add_executable(fapi_phy_pucch_adaptor_test ul_pucch_pdu_test.cpp)
target_link_libraries(fapi_phy_pucch_adaptor_test srsran_fapi_phy_message_adaptors gtest gtest_main srsran_fapi_message_builder_test_helpers srslog)
gtest_discover_tests(fapi_phy_pucch_adaptor_test)

add_executable(fapi_phy_pusch_adaptor_test ul_pusch_pdu_test.cpp)
target_link_libraries(fapi_phy_pusch_adaptor_test srsran_fapi_phy_message_adaptors srsran_fapi_uci_part2_tools gtest gtest_main srsran_fapi_message_builder_test_helpers srslog)
gtest_discover_tests(fapi_phy_pusch_adaptor_test)

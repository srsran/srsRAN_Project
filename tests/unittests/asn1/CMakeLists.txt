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

include_directories(../../..)

set_directory_properties(PROPERTIES LABELS "asn1")

add_executable(asn1_utils_test asn1_utils_test.cpp)
target_link_libraries(asn1_utils_test asn1_utils srsran_support gtest gtest_main)
add_test(asn1_utils_test asn1_utils_test)

add_executable(asn1_rrc_nr_test asn1_rrc_nr_test.cpp)
target_link_libraries(asn1_rrc_nr_test asn1_utils srsran_support rrc_nr_asn1 gtest gtest_main)
gtest_discover_tests(asn1_rrc_nr_test)

add_executable(asn1_e1ap_test asn1_e1ap_test.cpp)
target_link_libraries(asn1_e1ap_test srsran_pcap srsran_support e1ap_asn1 srslog srsran_pcap gtest gtest_main)
target_include_directories(asn1_e1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e1ap_test)

add_executable(asn1_e2ap_test asn1_e2ap_test.cpp)
target_link_libraries(asn1_e2ap_test e2ap_asn1 srslog srsran_support srsran_pcap gtest gtest_main)
target_include_directories(asn1_e2ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e2ap_test)

add_executable(asn1_f1ap_test asn1_f1ap_test.cpp)
target_link_libraries(asn1_f1ap_test f1ap_asn1 srslog srsran_support srsran_pcap gtest gtest_main)
target_include_directories(asn1_f1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_f1ap_test)

add_executable(asn1_ngap_test asn1_ngap_test.cpp)
target_link_libraries(asn1_ngap_test ngap_asn1 srslog srsran_support srsran_pcap gtest gtest_main)
gtest_discover_tests(asn1_ngap_test)

add_executable(asn1_cause_conversion_test asn1_cause_conversion_test.cpp)
target_link_libraries(asn1_cause_conversion_test ngap_asn1 f1ap_asn1 e1ap_asn1 srslog srsran_ran srsran_support gtest gtest_main)
gtest_discover_tests(asn1_cause_conversion_test)

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

set_directory_properties(PROPERTIES LABELS "e2ap")

set(SOURCES
    common/e2ap_asn1_packer.cpp
)
add_library(e2_test_helpers STATIC ${SOURCES})
target_link_libraries(e2_test_helpers srsran_e2 e2ap_asn1 srsran_support)

add_executable(e2_asn1_packer_test common/e2_asn1_packer_test.cpp)
target_link_libraries(e2_asn1_packer_test
                      srsran_support
                      srsran_e2
                      srslog
                      e2ap_asn1
                      srsran_ran
                      e2_test_helpers 		
                      gtest
                      gtest_main)
target_include_directories(e2_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_asn1_packer_test)

add_executable(e2_setup_procedure_test e2_setup_procedure_test.cpp)
target_link_libraries(e2_setup_procedure_test srsran_e2 srsran_support srslog e2ap_asn1 e2_test_helpers gtest gtest_main)
target_include_directories(e2_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_setup_procedure_test)

add_executable(e2_subscription_setup_test e2_subscription_setup_test.cpp)
target_link_libraries(e2_subscription_setup_test srsran_e2 srsran_support srslog e2ap_asn1 srsran_ran e2_test_helpers gtest gtest_main)
target_include_directories(e2_subscription_setup_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_subscription_setup_test)

add_executable(e2_ric_control_procedure_test e2_ric_control_procedure_test.cpp)
target_link_libraries(e2_ric_control_procedure_test srsran_e2 srsran_support srslog e2ap_asn1 srsran_ran e2_test_helpers gtest gtest_main)
target_include_directories(e2_ric_control_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_ric_control_procedure_test)

add_executable(e2ap_network_adapter_test e2ap_network_adapter_test.cpp dummy_ric.cpp)
target_link_libraries(e2ap_network_adapter_test srslog srsran_e2 e2ap_asn1 srsran_gateway srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(e2ap_network_adapter_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2ap_network_adapter_test)

add_executable(e2sm_kpm_meas_provider_test e2sm_kpm_meas_provider_test.cpp)
target_link_libraries(e2sm_kpm_meas_provider_test srslog srsran_e2 e2ap_asn1 srsran_pcap srsran_gateway srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(e2sm_kpm_meas_provider_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_meas_provider_test)

add_executable(e2sm_kpm_meas_provider_metrics_test e2sm_kpm_meas_provider_metrics_test.cpp)
target_link_libraries(e2sm_kpm_meas_provider_metrics_test srslog srsran_e2 e2ap_asn1 srsran_pcap srsran_gateway srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(e2sm_kpm_meas_provider_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_meas_provider_metrics_test)

add_executable(e2sm_kpm_test e2sm_kpm_test.cpp)
target_link_libraries(e2sm_kpm_test srslog srsran_e2 e2ap_asn1 srsran_pcap srsran_gateway srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(e2sm_kpm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2sm_kpm_test)

add_executable(e2_conn_upd_procedure_test e2_conn_upd_procedure_test.cpp)
target_link_libraries(e2_conn_upd_procedure_test srslog srsran_e2 e2ap_asn1 srsran_pcap srsran_gateway srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(e2_conn_upd_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e2_conn_upd_procedure_test)

add_executable(asn1_e2sm_ccc_test asn1_e2sm_ccc_test.cpp)
target_link_libraries(asn1_e2sm_ccc_test srslog srsran_e2 e2ap_asn1 srsran_pcap srsran_support e2_test_helpers gtest gtest_main)
target_include_directories(asn1_e2sm_ccc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e2sm_ccc_test)

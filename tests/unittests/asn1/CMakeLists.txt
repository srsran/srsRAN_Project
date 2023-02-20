#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "asn1")

add_executable(asn1_utils_test asn1_utils_test.cpp)
target_link_libraries(asn1_utils_test asn1_utils gtest gtest_main)
add_test(asn1_utils_test asn1_utils_test)

add_executable(asn1_rrc_nr_test asn1_rrc_nr_test.cpp)
target_link_libraries(asn1_rrc_nr_test asn1_utils rrc_nr_asn1 gtest gtest_main)
gtest_discover_tests(asn1_rrc_nr_test)

add_executable(asn1_e1ap_test asn1_e1ap_test.cpp)
target_link_libraries(asn1_e1ap_test e1ap_asn1 srslog srsgnb_pcap gtest gtest_main)
target_include_directories(asn1_e1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e1ap_test)

add_executable(asn1_e2ap_test asn1_e2ap_test.cpp)
target_link_libraries(asn1_e2ap_test e2ap_asn1 srslog srsgnb_pcap gtest gtest_main)
target_include_directories(asn1_e2ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_e2ap_test)

add_executable(asn1_f1ap_test asn1_f1ap_test.cpp)
target_link_libraries(asn1_f1ap_test f1ap_asn1 srslog srsgnb_pcap gtest gtest_main)
target_include_directories(asn1_f1ap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(asn1_f1ap_test)

add_executable(asn1_ngap_test asn1_ngap_test.cpp)
target_link_libraries(asn1_ngap_test ngap_asn1 srslog srsgnb_pcap gtest gtest_main)
gtest_discover_tests(asn1_ngap_test)

#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "asn1")

add_executable(asn1_utils_test asn1_utils_test.cpp)
target_link_libraries(asn1_utils_test asn1_utils)
add_test(asn1_utils_test asn1_utils_test)

add_executable(asn1_rrc_nr_test asn1_rrc_nr_test.cpp)
target_link_libraries(asn1_rrc_nr_test asn1_utils rrc_nr_asn1 gtest gtest_main)
gtest_discover_tests(asn1_rrc_nr_test)

add_executable(asn1_e1ap_test asn1_e1ap_test.cpp)
target_link_libraries(asn1_e1ap_test e1ap_asn1 srslog srsgnb_pcap gtest gtest_main)
gtest_discover_tests(asn1_e1ap_test)

add_executable(asn1_f1ap_test asn1_f1ap_test.cpp)
target_link_libraries(asn1_f1ap_test f1ap_asn1 srslog srsgnb_pcap gtest gtest_main)
gtest_discover_tests(asn1_f1ap_test)

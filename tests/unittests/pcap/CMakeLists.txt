#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pcap")

add_executable(pcap_mac_test mac_pcap_test.cpp)
target_link_libraries(pcap_mac_test srsran_pcap srsran_support srslog gtest gtest_main)
target_include_directories(pcap_mac_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_mac_test)

add_executable(pcap_rlc_test rlc_pcap_test.cpp)
target_link_libraries(pcap_rlc_test srsran_pcap srsran_support srslog gtest gtest_main)
target_include_directories(pcap_rlc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_rlc_test)

add_executable(pcap_ngap_test ngap_pcap_test.cpp)
target_link_libraries(pcap_ngap_test srsran_pcap srsran_support srslog gtest gtest_main)
target_include_directories(pcap_ngap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pcap_ngap_test)

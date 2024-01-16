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

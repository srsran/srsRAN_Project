#
# Copyright 2021-2023 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "mac")

include_directories(../../..)

add_library(mac_test_helpers mac_test_helpers.cpp mac_ctrl_test_dummies.cpp)
target_link_libraries(mac_test_helpers srsran_support srslog sched_config)

add_executable(mac_ue_create_test mac_ue_create_test.cpp)
target_link_libraries(mac_ue_create_test srsran_mac mac_test_helpers srsran_pcap srsran_support srslog gtest gtest_main)
gtest_discover_tests(mac_ue_create_test)

add_executable(mac_ctrl_test mac_ctrl_test.cpp)
target_link_libraries(mac_ctrl_test mac_test_helpers srsran_pcap srsran_support srslog rrc_nr_asn1 srsran_mac gtest gtest_main)
add_test(mac_ctrl_test mac_ctrl_test)

add_executable(mac_ul_pdu_test mac_ul_pdu_test.cpp)
target_link_libraries(mac_ul_pdu_test srsran_mac srsran_pcap srsran_support srslog gtest gtest_main)
add_test(mac_ul_pdu_test mac_ul_pdu_test)

add_executable(mac_ul_processor_test mac_ul_processor_test.cpp)
target_link_libraries(mac_ul_processor_test srsran_mac mac_test_helpers srsran_pcap srsran_support srslog gtest gtest_main)
add_test(mac_ul_processor_test mac_ul_processor_test)

add_executable(mac_dl_test
        mac_dl_cfg_test.cpp
        mac_rlf_detector_test.cpp
        mac_cell_processor_test.cpp
        dl_sch_pdu_assembler_test.cpp
        mac_rar_pdu_assembler_test.cpp)
target_link_libraries(mac_dl_test srsran_mac mac_test_helpers srsran_pcap sched_config gtest gtest_main)
gtest_discover_tests(mac_dl_test)
add_test(mac_dl_test mac_dl_test)

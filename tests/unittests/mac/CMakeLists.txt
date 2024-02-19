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

set_directory_properties(PROPERTIES LABELS "mac")

include_directories(../../..)

add_executable(mac_test
        mac_test_helpers.cpp
        mac_ctrl_test_dummies.cpp
        mac_ue_create_test.cpp
        mac_ctrl_test.cpp
        mac_ul_pdu_test.cpp
        mac_ul_processor_test.cpp
        mac_rlf_detector_test.cpp
        mac_cell_processor_test.cpp
        dl_sch_pdu_assembler_test.cpp
        mac_rar_pdu_assembler_test.cpp
        rnti_manager_test.cpp
        mac_dl_cfg_test.cpp)
target_link_libraries(mac_test
        srsran_pcap
        srsran_support
        srslog
        rrc_nr_asn1
        srsran_mac
        sched_config
        gtest
        gtest_main)
gtest_discover_tests(mac_test)
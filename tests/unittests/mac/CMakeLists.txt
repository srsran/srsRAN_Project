#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
        sib_pdu_assembler_test.cpp
        rnti_manager_test.cpp
        mac_rach_handler_test.cpp
        mac_dl_cfg_test.cpp
        mac_config_helpers_test.cpp)
target_link_libraries(mac_test
        srsran_pcap
        srsran_support
        srslog
        rrc_nr_asn1
        srsran_mac
        sched_config
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(mac_test)

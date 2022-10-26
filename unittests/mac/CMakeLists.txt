#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "mac")

include_directories(../..)

add_executable(mac_ue_create_test mac_ue_create_test.cpp)
target_link_libraries(mac_ue_create_test srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(mac_ue_create_test)

add_executable(mac_ctrl_test mac_ctrl_test.cpp)
target_link_libraries(mac_ctrl_test srsgnb_support srslog rrc_nr_asn1 srsgnb_mac)
add_test(mac_ctrl_test mac_ctrl_test)

add_executable(mac_dl_cfg_test mac_dl_cfg_test.cpp)
target_link_libraries(mac_dl_cfg_test
                      srsgnb_mac
                      srsgnb_support
                      srslog
                      sched_config
                      rrc_nr_asn1)
add_test(mac_dl_cfg_test mac_dl_cfg_test)

add_executable(mac_ul_pdu_test mac_ul_pdu_test.cpp)
target_link_libraries(mac_ul_pdu_test srsgnb_mac srsgnb_support srslog gtest gtest_main)
add_test(mac_ul_pdu_test mac_ul_pdu_test)

add_executable(mac_ul_processor_test mac_ul_processor_test.cpp)
target_link_libraries(mac_ul_processor_test srsgnb_mac srsgnb_support srslog gtest gtest_main)
add_test(mac_ul_processor_test mac_ul_processor_test)

add_executable(mac_rar_pdu_assembler_test mac_rar_pdu_assembler_test.cpp)
target_link_libraries(mac_rar_pdu_assembler_test srsgnb_mac srsgnb_support srslog sched_config)
add_test(mac_rar_pdu_assembler_test mac_rar_pdu_assembler_test)

add_executable(dl_sch_pdu_assembler_test dl_sch_pdu_assembler_test.cpp)
target_link_libraries(dl_sch_pdu_assembler_test srsgnb_mac srsgnb_support srslog sched_config gtest gtest_main)
add_test(dl_sch_pdu_assembler_test dl_sch_pdu_assembler_test)

add_executable(mac_cell_processor_test mac_cell_processor_test.cpp)
target_link_libraries(mac_cell_processor_test srsgnb_mac sched_config gtest gtest_main)
add_test(mac_cell_processor_test mac_cell_processor_test)
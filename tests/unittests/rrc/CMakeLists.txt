#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "rrc")

add_library(rrc_ue_test_helpers rrc_ue_test_messages.cpp)
target_include_directories(rrc_ue_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test_helpers srsran_support srslog)

set(SOURCES
        rrc_asn1_helpers_test.cpp
        rrc_ue_setup_proc_test.cpp
        rrc_ue_dl_info_transfer_proc_test.cpp
        rrc_ue_smc_proc_test.cpp
        rrc_ue_reconfig_proc_test.cpp
        rrc_ue_capability_transfer_proc_test.cpp
        rrc_ue_reest_proc_test.cpp
        rrc_ue_meas_report_test.cpp)

add_executable(rrc_ue_test ${SOURCES})
target_include_directories(rrc_ue_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test rrc_ue_test_helpers srsran_e1ap srsran_rrc srsran_cu_cp_cell_meas_manager srsran_support srslog gtest gtest_main)
gtest_discover_tests(rrc_ue_test PROPERTIES "LABELS;rrc_ue")

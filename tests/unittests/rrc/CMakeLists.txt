#
# Copyright 2021-2023 Software Radio Systems Limited
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
        rrc_ue_setup_proc_test.cpp
        rrc_ue_smc_proc_test.cpp
        rrc_ue_reconfig_proc_test.cpp)

add_executable(rrc_ue_test ${SOURCES})
target_include_directories(rrc_ue_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test rrc_ue_test_helpers srsran_e1ap srsran_rrc srsran_support srslog gtest gtest_main)
gtest_discover_tests(rrc_ue_test PROPERTIES "LABELS;rrc_ue")

add_executable(drb_manager_test drb_manager_test.cpp)
target_link_libraries(drb_manager_test rrc_ue_test_helpers du_processor_test_helpers srsran_rrc srsran_e1ap srsran_support srslog gtest gtest_main)
gtest_discover_tests(drb_manager_test)

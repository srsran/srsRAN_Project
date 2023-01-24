#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "rrc")

add_library(rrc_ue_test_helpers rrc_ue_test_messages.cpp)
target_include_directories(rrc_ue_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test_helpers srsgnb_support srslog)

add_executable(rrc_ue_setup_proc_test rrc_ue_setup_proc_test.cpp)
target_link_libraries(rrc_ue_setup_proc_test rrc_ue_test_helpers srsgnb_rrc srsgnb_e1 srsgnb_support srslog gtest gtest_main)
target_include_directories(rrc_ue_setup_proc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rrc_ue_setup_proc_test)

add_executable(rrc_ue_smc_proc_test rrc_ue_smc_proc_test.cpp)
target_link_libraries(rrc_ue_smc_proc_test rrc_ue_test_helpers srsgnb_rrc srsgnb_e1 srsgnb_support srslog gtest gtest_main)
target_include_directories(rrc_ue_smc_proc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rrc_ue_smc_proc_test)

add_executable(rrc_ue_reconfig_proc_test rrc_ue_reconfig_proc_test.cpp)
target_link_libraries(rrc_ue_reconfig_proc_test rrc_ue_test_helpers srsgnb_e1 srsgnb_rrc srsgnb_support srslog gtest gtest_main)
target_include_directories(rrc_ue_reconfig_proc_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rrc_ue_reconfig_proc_test)

add_executable(drb_manager_test drb_manager_test.cpp)
target_link_libraries(drb_manager_test rrc_ue_test_helpers srsgnb_rrc srsgnb_e1 srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(drb_manager_test)

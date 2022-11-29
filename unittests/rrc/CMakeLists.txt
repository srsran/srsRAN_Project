#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "rrc")

add_executable(rrc_ue_setup_proc_test rrc_ue_setup_proc_test.cpp)
target_link_libraries(rrc_ue_setup_proc_test srsgnb_rrc srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(rrc_ue_setup_proc_test)

add_executable(rrc_ue_smc_proc_test rrc_ue_smc_proc_test.cpp)
target_link_libraries(rrc_ue_smc_proc_test srsgnb_rrc srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(rrc_ue_smc_proc_test)

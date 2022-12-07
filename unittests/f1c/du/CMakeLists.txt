#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_library(f1_du_test_helpers f1_du_test_helpers.cpp)
target_link_libraries(f1_du_test_helpers srsgnb_f1c_du srsgnb_support du_manager_converters sched_config)

add_executable(f1_du_setup_procedure_test f1_du_setup_procedure_test.cpp)
target_link_libraries(f1_du_setup_procedure_test
        srsgnb_f1c_du
        srsgnb_support
        du_manager_converters
        srsgnb_ran
        sched_config
        f1_du_test_helpers
        gtest
        gtest_main)
target_include_directories(f1_du_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_du_setup_procedure_test)

add_executable(f1_du_dl_rrc_message_transfer_test f1_du_dl_rrc_message_transfer_test.cpp)
target_link_libraries(f1_du_dl_rrc_message_transfer_test
        srsgnb_f1c_du
        srsgnb_support
        du_manager_converters
        srsgnb_ran
        sched_config
        f1_du_test_helpers
        gtest
        gtest_main)

add_executable(f1_du_ue_config_test f1_du_ue_config_test.cpp)
target_link_libraries(f1_du_ue_config_test
        srsgnb_f1c_du
        srsgnb_support
        du_manager_converters
        srsgnb_ran
        sched_config
        f1_du_test_helpers
        gtest
        gtest_main)
gtest_discover_tests(f1_du_ue_config_test)

add_executable(f1_du_ue_context_setup_test f1_du_ue_context_setup_procedure_test.cpp)
target_link_libraries(f1_du_ue_context_setup_test
        srsgnb_f1c_du
        srsgnb_support
        du_manager_converters
        srsgnb_ran
        sched_config
        f1_du_test_helpers
        gtest
        gtest_main)

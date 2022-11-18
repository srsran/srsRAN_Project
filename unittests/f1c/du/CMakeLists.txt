#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_executable(f1_du_setup_procedure_test f1_du_setup_procedure_test.cpp)
target_link_libraries(f1_du_setup_procedure_test
        srsgnb_f1c
        srsgnb_support
        srslog
        f1ap_asn1
        du_manager_converters
        srsgnb_ran
        sched_config
        gtest
        gtest_main)
target_include_directories(f1_du_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_du_setup_procedure_test)

add_executable(f1_du_dl_rrc_message_transfer_test f1_du_dl_rrc_message_transfer_test.cpp)
target_link_libraries(f1_du_dl_rrc_message_transfer_test
        srsgnb_f1c
        srsgnb_support
        srslog
        f1ap_asn1
        du_manager_converters
        srsgnb_ran
        sched_config
        gtest
        gtest_main)


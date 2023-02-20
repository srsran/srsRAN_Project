#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_library(f1ap_du_test_helpers f1ap_du_test_helpers.cpp)
target_link_libraries(f1ap_du_test_helpers f1ap_test_helpers srsgnb_f1ap_du srsgnb_support du_manager_converters sched_config)

set(SOURCES f1ap_du_setup_procedure_test.cpp
            f1ap_du_dl_rrc_message_transfer_test.cpp
            f1ap_du_ue_config_test.cpp
            f1ap_du_ue_context_setup_procedure_test.cpp
            f1ap_du_ue_context_modification_test.cpp)

add_executable(f1ap_du_test ${SOURCES})
target_include_directories(f1ap_du_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_du_test
        srsgnb_f1ap_du
        srsgnb_support
        du_manager_converters
        srsgnb_ran
        sched_config
        f1ap_du_test_helpers
        gtest
        gtest_main)
gtest_discover_tests(f1ap_du_test PROPERTIES "LABELS;f1ap_du")

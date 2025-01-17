#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

set_directory_properties(PROPERTIES LABELS "f1ap;f1ap_du")

add_library(f1ap_du_test_helpers f1ap_du_test_helpers.cpp
        f1ap_du_reset_test.cpp
)
target_link_libraries(f1ap_du_test_helpers f1ap_test_helpers f1ap_test_doubles
        srsran_f1ap_du srsran_support du_manager_converters sched_config)

set(SOURCES f1ap_du_setup_procedure_test.cpp
            f1ap_du_dl_rrc_message_transfer_test.cpp
            f1ap_du_ue_config_test.cpp
            f1ap_du_ue_context_setup_procedure_test.cpp
            f1ap_du_ue_context_release_test.cpp
            f1ap_du_ue_context_modification_test.cpp
            f1ap_du_reset_test.cpp
            f1ap_du_ul_rrc_message_transfer_test.cpp
            f1ap_du_gnbdu_config_update_test.cpp
)

add_executable(f1ap_du_test ${SOURCES})
target_include_directories(f1ap_du_test PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(f1ap_du_test PROPERTIES UNITY_BUILD ON)
target_link_libraries(f1ap_du_test
        srsran_f1ap_du
        srsran_support
        du_manager_converters
        srsran_ran
        sched_config
        f1ap_du_test_helpers
        gtest
        gtest_main)
add_test(f1ap_du_test f1ap_du_test)

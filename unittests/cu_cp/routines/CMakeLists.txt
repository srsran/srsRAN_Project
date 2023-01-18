#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
set_directory_properties(PROPERTIES LABELS "cu_cp")

add_executable(du_processor_pdu_sessions_setup_routine_test du_processor_pdu_sessions_setup_routine_test.cpp)
target_link_libraries(du_processor_pdu_sessions_setup_routine_test f1ap_test_helpers cu_cp_test_helpers srsgnb_cu_cp srsgnb_rrc srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(du_processor_pdu_sessions_setup_routine_test)
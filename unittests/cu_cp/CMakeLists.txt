#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(cu_cp_test_helpers test_helpers.cpp)
target_link_libraries(cu_cp_test_helpers srsgnb_cu_cp srsgnb_support srslog f1ap_asn1 ngap_asn1)
target_include_directories(cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(cu_cp_test cu_cp_test.cpp)
target_link_libraries(cu_cp_test cu_cp_test_helpers f1_cu_test_helpers srsgnb_cu_cp srsgnb_support srslog f1ap_asn1 ngap_asn1 gtest gtest_main)
target_include_directories(cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_test)

add_executable(cu_cp_config_test cu_cp_config_test.cpp)
target_link_libraries(cu_cp_config_test cu_cp_test_helpers srsgnb_cu_cp gtest gtest_main)
target_include_directories(cu_cp_config_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_config_test)

add_executable(du_processor_test du_processor_test.cpp)
target_link_libraries(du_processor_test f1_test_helpers cu_cp_test_helpers srsgnb_cu_cp srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(du_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(du_processor_test)

add_executable(du_processor_pdu_sessions_setup_routine_test du_processor_pdu_sessions_setup_routine_test.cpp)
target_link_libraries(du_processor_pdu_sessions_setup_routine_test f1_test_helpers cu_cp_test_helpers srsgnb_cu_cp srsgnb_rrc srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(du_processor_pdu_sessions_setup_routine_test)
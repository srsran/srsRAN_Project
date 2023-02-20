#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
set_directory_properties(PROPERTIES LABELS "cu_up")

add_executable(cu_up_test cu_up_test.cpp)
target_link_libraries(cu_up_test srsran_cu_up srsran_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(cu_up_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_test)

add_executable(cu_up_ue_manager_test ue_manager_test.cpp)
target_link_libraries(cu_up_ue_manager_test srsran_cu_up srsran_support srslog gtest gtest_main)
target_include_directories(cu_up_ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_ue_manager_test)

add_executable(pdu_session_manager_test pdu_session_manager_test.cpp)
target_link_libraries(pdu_session_manager_test srsran_cu_up srsran_support e1ap_asn1 srslog gtest gtest_main)
target_include_directories(pdu_session_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdu_session_manager_test)

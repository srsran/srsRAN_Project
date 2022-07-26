#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(f1c_asn1_helpers_test common/f1c_asn1_helpers_test.cpp)
target_link_libraries(f1c_asn1_helpers_test srsgnb_f1 srsgnb_support srslog f1ap_asn1)
target_include_directories(f1c_asn1_helpers_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1c_asn1_helpers_test f1c_asn1_helpers_test)

add_executable(f1_du_setup_procedure_test du/f1_du_setup_procedure_test.cpp)
target_link_libraries(f1_du_setup_procedure_test
                      srsgnb_f1
                      srsgnb_support
                      srslog
                      f1ap_asn1
                      du_manager_converters
                      srsgnb_ran)
target_include_directories(f1_du_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1_du_setup_procedure_test f1_du_setup_procedure_test)

add_executable(f1_cu_setup_procedure_test cu/f1_cu_setup_procedure_test.cpp)
target_link_libraries(f1_cu_setup_procedure_test srsgnb_f1 srsgnb_support srslog f1ap_asn1)
target_include_directories(f1_cu_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1_cu_setup_procedure_test f1_cu_setup_procedure_test)

add_executable(f1_cu_ue_context_release_procedure_test cu/f1_cu_ue_context_release_procedure_test.cpp)
target_link_libraries(f1_cu_ue_context_release_procedure_test srsgnb_f1 srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1_cu_ue_context_release_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_cu_ue_context_release_procedure_test)

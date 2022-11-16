#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########## CU-CP ##########

add_executable(e1_cu_cp_setup_procedure_test cu_cp/e1_cu_cp_setup_procedure_test.cpp)
target_link_libraries(e1_cu_cp_setup_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_cp_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_cp_setup_procedure_test)

add_executable(e1_cu_cp_bearer_context_setup_procedure_test cu_cp/e1_cu_cp_bearer_context_setup_procedure_test.cpp)
target_link_libraries(e1_cu_cp_bearer_context_setup_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_cp_bearer_context_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_cp_bearer_context_setup_procedure_test)

add_executable(e1_cu_cp_bearer_context_modification_procedure_test cu_cp/e1_cu_cp_bearer_context_modification_procedure_test.cpp)
target_link_libraries(e1_cu_cp_bearer_context_modification_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_cp_bearer_context_modification_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_cp_bearer_context_modification_procedure_test)

add_executable(e1_cu_cp_bearer_context_release_procedure_test cu_cp/e1_cu_cp_bearer_context_release_procedure_test.cpp)
target_link_libraries(e1_cu_cp_bearer_context_release_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_cp_bearer_context_release_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_cp_bearer_context_release_procedure_test)

add_executable(e1_cu_cp_test cu_cp/e1_cu_cp_test.cpp)
target_link_libraries(e1_cu_cp_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_cp_test)

########## CU-UP ##########

add_executable(e1_cu_up_test cu_up/e1_cu_up_test.cpp)
target_link_libraries(e1_cu_up_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_up_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_up_test)

add_executable(e1_cu_up_setup_procedure_test cu_up/e1_cu_up_setup_procedure_test.cpp)
target_link_libraries(e1_cu_up_setup_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_up_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_up_setup_procedure_test)

add_executable(e1_cu_up_bearer_context_setup_procedure_test cu_up/e1_cu_up_bearer_context_setup_procedure_test.cpp)
target_link_libraries(e1_cu_up_bearer_context_setup_procedure_test srsgnb_e1 srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(e1_cu_up_bearer_context_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_cu_up_bearer_context_setup_procedure_test)

########## Common #########

add_executable(e1_asn1_packer_test common/e1_asn1_packer_test.cpp)
target_link_libraries(e1_asn1_packer_test
                      srsgnb_e1
                      srsgnb_support
                      srslog
                      e1ap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(e1_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1_asn1_packer_test)
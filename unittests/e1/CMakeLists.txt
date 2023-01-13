#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "e1ap")

include_directories(../..)

add_subdirectory(common)
add_subdirectory(cu_cp)
# add_subdirectory(cu_up)

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

#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "f1ap")

include_directories(../..)

add_executable(f1c_asn1_helpers_test common/f1c_asn1_helpers_test.cpp)
target_link_libraries(f1c_asn1_helpers_test srsgnb_f1c_common srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1c_asn1_helpers_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1c_asn1_helpers_test f1c_asn1_helpers_test)
gtest_discover_tests(f1c_asn1_helpers_test)

add_executable(f1_cu_setup_procedure_test cu_cp/f1_cu_setup_procedure_test.cpp)
target_link_libraries(f1_cu_setup_procedure_test srsgnb_f1c_cu srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1_cu_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_cu_setup_procedure_test)

add_executable(f1_cu_ue_context_setup_procedure_test cu_cp/f1_cu_ue_context_setup_procedure_test.cpp)
target_link_libraries(f1_cu_ue_context_setup_procedure_test srsgnb_f1c_cu srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1_cu_ue_context_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_cu_ue_context_setup_procedure_test)

add_executable(f1_cu_ue_context_release_procedure_test cu_cp/f1_cu_ue_context_release_procedure_test.cpp)
target_link_libraries(f1_cu_ue_context_release_procedure_test srsgnb_f1c_cu srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1_cu_ue_context_release_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_cu_ue_context_release_procedure_test)

add_executable(f1_cu_ue_context_modification_procedure_test cu_cp/f1_cu_ue_context_modification_procedure_test.cpp)
target_link_libraries(f1_cu_ue_context_modification_procedure_test srsgnb_f1c_cu srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1_cu_ue_context_modification_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1_cu_ue_context_modification_procedure_test)

add_executable(f1c_cu_test cu_cp/f1c_cu_test.cpp)
target_link_libraries(f1c_cu_test srsgnb_f1c_cu srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(f1c_cu_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1c_cu_test)

add_executable(f1c_asn1_packer_test common/f1c_asn1_packer_test.cpp)
target_link_libraries(f1c_asn1_packer_test
                      srsgnb_f1c_common
                      srsgnb_support
                      srslog
                      f1ap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(f1c_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1c_asn1_packer_test)

add_subdirectory(du)

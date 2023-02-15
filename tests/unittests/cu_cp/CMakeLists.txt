#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
set_directory_properties(PROPERTIES LABELS "cu_cp")

include_directories(../../..)

add_subdirectory(common)
add_subdirectory(du_processor)
add_subdirectory(cu_up_processor)
add_subdirectory(ue_manager)

add_executable(cu_cp_test cu_cp_test.cpp)
target_link_libraries(cu_cp_test cu_cp_test_helpers f1ap_cu_test_helpers ngap_test_helpers srsgnb_cu_cp srsgnb_support srslog f1ap_asn1 ngap_asn1 gtest gtest_main)
target_include_directories(cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_test)

add_executable(cu_cp_config_test cu_cp_config_test.cpp)
target_link_libraries(cu_cp_config_test cu_cp_test_helpers srsgnb_cu_cp gtest gtest_main)
target_include_directories(cu_cp_config_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_config_test)

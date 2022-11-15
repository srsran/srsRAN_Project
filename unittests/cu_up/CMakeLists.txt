#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_up_test cu_up_test.cpp)
target_link_libraries(cu_up_test srsgnb_cu_up srsgnb_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(cu_up_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_test)

add_executable(cu_up_ue_manager_test ue_manager_test.cpp)
target_link_libraries(cu_up_ue_manager_test srsgnb_cu_up srsgnb_support srslog gtest gtest_main)
target_include_directories(cu_up_ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_ue_manager_test)

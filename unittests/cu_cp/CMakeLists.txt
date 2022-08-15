#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_cp_test cu_cp_test.cpp)
target_link_libraries(cu_cp_test srsgnb_cu_cp srsgnb_f1 srsgnb_support srslog f1ap_asn1 ngap_asn1 gtest gtest_main)
target_include_directories(cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_test)

add_executable(du_processor_test du_processor_test.cpp)
target_link_libraries(du_processor_test srsgnb_cu_cp srsgnb_f1 srsgnb_support srslog f1ap_asn1 gtest gtest_main)
target_include_directories(du_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(du_processor_test)

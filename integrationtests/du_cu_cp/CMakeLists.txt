#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_du_test cu_du_test.cpp)
target_link_libraries(cu_du_test srsgnb_cu_cp srsgnb_du_high srsgnb_f1c srsgnb_rrc srsgnb_support srslog f1ap_asn1 ngap_asn1 gtest gtest_main)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_du_test)

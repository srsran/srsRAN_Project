#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_du_test cu_du_test.cpp)
target_link_libraries(cu_du_test srsran_cu_cp srsran_du_high srsran_f1ap_cu srsran_rrc srsran_support srslog f1ap_asn1 ngap_asn1 gtest gtest_main)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_du_test)

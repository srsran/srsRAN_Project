#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(e2ap_integration_test e2ap_integration_test.cpp)
target_link_libraries(e2ap_integration_test srslog srsran_e2 e2ap_asn1 e2_test_helpers srsran_gateway srsran_support gtest gtest_main)
target_include_directories(e2ap_integration_test PRIVATE ${CMAKE_SOURCE_DIR})
#gtest_discover_tests(e2ap_integration_test)

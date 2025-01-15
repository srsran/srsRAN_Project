#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(e1_gateway_test e1_gateway_test.cpp)
target_link_libraries(e1_gateway_test
        srsran_e1_gateway
        srsran_gateway
        e1ap_test_helpers
        srsran_e1ap_common
        srsran_support
        srsran_network
        srslog
        e1ap_asn1
        gtest
        gtest_main)
add_test(e1_gateway_test e1_gateway_test)

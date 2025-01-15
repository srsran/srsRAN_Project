#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(f1c_gateway_test f1c_gateway_test.cpp)
target_link_libraries(f1c_gateway_test
        srsran_f1c_gateway
        srsran_gateway
        f1ap_test_helpers
        srsran_f1ap_common
        srsran_support
        srsran_network
        srslog
        f1ap_asn1
        gtest
        gtest_main)
add_test(f1c_gateway_test f1c_gateway_test)

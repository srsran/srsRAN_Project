#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "f1ap")

include_directories(../../..)

add_executable(f1c_gateway_test f1c_gateway_test.cpp)
target_link_libraries(f1c_gateway_test
        srsran_f1ap_du
        srsran_f1ap_cu
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

add_subdirectory(common)
add_subdirectory(du)
add_subdirectory(cu_cp)


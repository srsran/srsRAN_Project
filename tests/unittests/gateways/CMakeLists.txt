#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "gateways")

add_executable(sctp_network_gateway_test
        sctp_network_server_test.cpp
        sctp_network_client_test.cpp
        sctp_network_gateway_test.cpp
        sctp_network_link_test.cpp)
target_link_libraries(sctp_network_gateway_test srsran_gateway srsran_support srslog gtest gtest_main)
add_test(sctp_network_gateway_test sctp_network_gateway_test)
set_tests_properties(sctp_network_gateway_test PROPERTIES LABELS "tsan")

add_executable(udp_network_gateway_test udp_network_gateway_test.cpp)
target_link_libraries(udp_network_gateway_test srsran_gateway srsran_support srslog gtest gtest_main)
add_test(udp_network_gateway_test udp_network_gateway_test)
set_tests_properties(udp_network_gateway_test PROPERTIES LABELS "tsan")

add_executable(udp_network_gateway_pool_depletion_test udp_network_gateway_pool_depletion_test.cpp)
target_link_libraries(udp_network_gateway_pool_depletion_test srsran_gateway srsran_support srslog gtest gtest_main)
add_test(udp_network_gateway_pool_depletion_test udp_network_gateway_pool_depletion_test)
set_tests_properties(udp_network_gateway_pool_depletion_test PROPERTIES LABELS "tsan")

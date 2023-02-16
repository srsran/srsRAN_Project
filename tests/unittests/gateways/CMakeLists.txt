#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "gateways")

add_executable(sctp_network_gateway_test sctp_network_gateway_test.cpp)
target_link_libraries(sctp_network_gateway_test srsgnb_gateway srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(sctp_network_gateway_test)

add_executable(udp_network_gateway_test udp_network_gateway_test.cpp)
target_link_libraries(udp_network_gateway_test srsgnb_gateway srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(udp_network_gateway_test)

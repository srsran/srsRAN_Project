#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "gateways")

include_directories(../../..)

add_executable(udp_network_gateway_benchmark udp_network_gateway_benchmark.cpp)
target_link_libraries(udp_network_gateway_benchmark srsran_network srsran_gateway srsran_support srslog)

add_executable(udp_network_gateway_tx_benchmark udp_network_gateway_tx_benchmark.cpp)
target_link_libraries(udp_network_gateway_tx_benchmark srsran_network srsran_gateway srsran_support srslog)

add_executable(udp_network_gateway_rx_benchmark udp_network_gateway_rx_benchmark.cpp)
target_link_libraries(udp_network_gateway_rx_benchmark srsran_network srsran_gateway srsran_support srslog)

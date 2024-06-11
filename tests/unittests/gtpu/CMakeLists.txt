#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "gtpu")

include_directories(../../..)

add_executable(gtpu_test gtpu_test.cpp)
target_link_libraries(gtpu_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_test)

add_executable(gtpu_demux_test gtpu_demux_test.cpp)
target_link_libraries(gtpu_demux_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_demux_test)

add_executable(gtpu_echo_test gtpu_echo_test.cpp)
target_link_libraries(gtpu_echo_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_echo_test)

add_executable(gtpu_tunnel_ngu_test gtpu_tunnel_ngu_test.cpp)
target_link_libraries(gtpu_tunnel_ngu_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_tunnel_ngu_test)

add_executable(gtpu_tunnel_ngu_rx_test gtpu_tunnel_ngu_rx_test.cpp)
target_link_libraries(gtpu_tunnel_ngu_rx_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_tunnel_ngu_rx_test)

add_executable(gtpu_tunnel_ngu_tx_test gtpu_tunnel_ngu_tx_test.cpp)
target_link_libraries(gtpu_tunnel_ngu_tx_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_tunnel_ngu_tx_test)

add_executable(gtpu_tunnel_nru_test gtpu_tunnel_nru_test.cpp)
target_link_libraries(gtpu_tunnel_nru_test srsran_gtpu srsran_pcap srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_tunnel_nru_test)

add_executable(gtpu_tunnel_nru_rx_test gtpu_tunnel_nru_rx_test.cpp)
target_link_libraries(gtpu_tunnel_nru_rx_test srsran_gtpu srsran_pcap srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_tunnel_nru_rx_test)

add_executable(gtpu_alloc_test gtpu_test_allocator.cpp)
target_link_libraries(gtpu_alloc_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_alloc_test)

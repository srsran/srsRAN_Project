#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pdcp")

include_directories(../../..)

add_executable(pdcp_tx_benchmark pdcp_tx_benchmark.cpp)
target_link_libraries(pdcp_tx_benchmark srsran_pdcp srsran_security srslog)
add_test(pdcp_tx_benchmark_nea0 pdcp_tx_benchmark -a0 -R3)
add_test(pdcp_tx_benchmark_nea1 pdcp_tx_benchmark -a1 -R1)
add_test(pdcp_tx_benchmark_nea2 pdcp_tx_benchmark -a2 -R3)
add_test(pdcp_tx_benchmark_nea3 pdcp_tx_benchmark -a3 -R1)
set_tests_properties(pdcp_tx_benchmark_nea0 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea1 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea2 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_tx_benchmark_nea3 PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_benchmark pdcp_rx_benchmark.cpp)
target_link_libraries(pdcp_rx_benchmark srsran_pdcp srsran_security srslog)
add_test(pdcp_rx_benchmark_nea0 pdcp_rx_benchmark -a0 -R3)
add_test(pdcp_rx_benchmark_nea1 pdcp_rx_benchmark -a1 -R1)
add_test(pdcp_rx_benchmark_nea2 pdcp_rx_benchmark -a2 -R3)
add_test(pdcp_rx_benchmark_nea3 pdcp_rx_benchmark -a3 -R1)
set_tests_properties(pdcp_rx_benchmark_nea0 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea1 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea2 PROPERTIES LABELS "tsan")
set_tests_properties(pdcp_rx_benchmark_nea3 PROPERTIES LABELS "tsan")

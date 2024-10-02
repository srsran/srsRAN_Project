#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pdcp")

include_directories(../../..)

add_executable(pdcp_tx_benchmark pdcp_tx_benchmark.cpp)
target_link_libraries(pdcp_tx_benchmark srsran_pdcp srsran_security srslog)
add_test(pdcp_tx_benchmark pdcp_tx_benchmark)
set_tests_properties(pdcp_tx_benchmark PROPERTIES LABELS "tsan")

add_executable(pdcp_rx_benchmark pdcp_rx_benchmark.cpp)
target_link_libraries(pdcp_rx_benchmark srsran_pdcp srsran_security srslog)
add_test(pdcp_rx_benchmark pdcp_rx_benchmark)
set_tests_properties(pdcp_rx_benchmark PROPERTIES LABELS "tsan")

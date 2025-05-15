#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_executable(rlc_handle_status_report rlc_handle_status_report.cpp)
target_link_libraries(rlc_handle_status_report srsran_rlc srslog pdcp_test_doubles)
add_test(rlc_handle_status_report rlc_handle_status_report)

add_executable(rlc_am_rx_benchmark rlc_am_rx_benchmark.cpp)
target_link_libraries(rlc_am_rx_benchmark srsran_rlc srslog pdcp_test_doubles)
add_test(rlc_am_rx_benchmark rlc_am_rx_benchmark)

add_executable(rlc_am_tx_benchmark rlc_am_tx_benchmark.cpp)
target_link_libraries(rlc_am_tx_benchmark srsran_rlc srslog pdcp_test_doubles)
add_test(rlc_am_tx_benchmark rlc_am_tx_benchmark)

set_tests_properties(rlc_am_tx_benchmark rlc_am_rx_benchmark PROPERTIES LABELS "rlc|rtsan")
set_tests_properties(rlc_handle_status_report PROPERTIES LABELS "rlc")

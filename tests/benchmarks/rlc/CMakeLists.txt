#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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

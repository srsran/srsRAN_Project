#
# Copyright 2021-2024 Software Radio Systems Limited
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

add_executable(gtpu_alloc_test gtpu_test_allocator.cpp)
target_link_libraries(gtpu_alloc_test srsran_gtpu srsran_support srslog gtest gtest_main)
gtest_discover_tests(gtpu_alloc_test)

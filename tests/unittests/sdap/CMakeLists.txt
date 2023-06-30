#
# Copyright 2021-2023 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(sdap_test sdap_test.cpp)
target_link_libraries(sdap_test srsran_support srslog gtest gtest_main)
target_include_directories(sdap_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_test)

add_executable(sdap_tx_test sdap_tx_test.cpp)
target_link_libraries(sdap_tx_test srsran_support srslog gtest gtest_main)
target_include_directories(sdap_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_tx_test)

add_executable(sdap_rx_test sdap_rx_test.cpp)
target_link_libraries(sdap_rx_test srsran_support srslog gtest gtest_main)
target_include_directories(sdap_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_rx_test)

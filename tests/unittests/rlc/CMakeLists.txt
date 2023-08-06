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

set_directory_properties(PROPERTIES LABELS "rlc")

add_executable(rlc_rx_am_test rlc_rx_am_test.cpp)
target_link_libraries(rlc_rx_am_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_rx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_am_test)

add_executable(rlc_tx_am_test rlc_tx_am_test.cpp)
target_link_libraries(rlc_tx_am_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_tx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_am_test)

add_executable(rlc_rx_tm_test rlc_rx_tm_test.cpp)
target_link_libraries(rlc_rx_tm_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_rx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_tm_test)

add_executable(rlc_tx_tm_test rlc_tx_tm_test.cpp)
target_link_libraries(rlc_tx_tm_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_tx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_tm_test)

add_executable(rlc_um_test rlc_um_test.cpp)
target_link_libraries(rlc_um_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_um_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_um_test)

add_executable(rlc_um_pdu_test rlc_um_pdu_test.cpp)
target_link_libraries(rlc_um_pdu_test srsran_support srslog)
target_include_directories(rlc_um_pdu_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(rlc_um_pdu_test rlc_um_pdu_test)

add_executable(rlc_am_pdu_test rlc_am_pdu_test.cpp)
target_link_libraries(rlc_am_pdu_test srsran_rlc srsran_support srslog)
target_include_directories(rlc_am_pdu_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(rlc_am_pdu_test rlc_am_pdu_test)

add_executable(rlc_pdu_recycler_test rlc_pdu_recycler_test.cpp)
target_link_libraries(rlc_pdu_recycler_test srsran_support srslog gtest gtest_main)
target_include_directories(rlc_pdu_recycler_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_pdu_recycler_test)

add_executable(rlc_sdu_queue_test rlc_sdu_queue_test.cpp)
target_link_libraries(rlc_sdu_queue_test srsran_support srslog)
target_include_directories(rlc_sdu_queue_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(rlc_sdu_queue_test rlc_sdu_queue_test)

add_executable(rlc_retx_queue_test rlc_retx_queue_test.cpp)
target_link_libraries(rlc_retx_queue_test srsran_support srslog gtest gtest_main)
target_include_directories(rlc_retx_queue_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(rlc_retx_queue_test rlc_retx_queue_test)

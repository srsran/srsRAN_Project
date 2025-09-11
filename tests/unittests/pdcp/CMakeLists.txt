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

set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(pdcp_tx_test pdcp_tx_test.cpp)
target_link_libraries(pdcp_tx_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_test pdcp_tx_test)
set_tests_properties(pdcp_tx_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_tx_test)

add_executable(pdcp_tx_empty_pool_test pdcp_tx_empty_pool_test.cpp)
target_link_libraries(pdcp_tx_empty_pool_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_empty_pool_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_empty_pool_test pdcp_tx_empty_pool_test)
set_tests_properties(pdcp_tx_empty_pool_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_tx_empty_pool_test)

add_executable(pdcp_rx_test pdcp_rx_test.cpp)
target_link_libraries(pdcp_rx_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_rx_test pdcp_rx_test)
set_tests_properties(pdcp_rx_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_rx_test)

add_executable(pdcp_tx_metrics_test pdcp_tx_metrics_test.cpp)
target_link_libraries(pdcp_tx_metrics_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_metrics_test pdcp_tx_metrics_test)
set_tests_properties(pdcp_tx_metrics_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_tx_metrics_test)

add_executable(pdcp_rx_metrics_test pdcp_rx_metrics_test.cpp)
target_link_libraries(pdcp_rx_metrics_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_rx_metrics_test pdcp_rx_metrics_test)
set_tests_properties(pdcp_rx_metrics_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_rx_metrics_test)

add_executable(pdcp_tx_status_report_test pdcp_tx_status_report_test.cpp)
target_link_libraries(pdcp_tx_status_report_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_status_report_test pdcp_tx_status_report_test)
set_tests_properties(pdcp_tx_status_report_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_tx_status_report_test)

add_executable(pdcp_rx_status_report_test pdcp_rx_status_report_test.cpp)
target_link_libraries(pdcp_rx_status_report_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_rx_status_report_test pdcp_rx_status_report_test)
set_tests_properties(pdcp_rx_status_report_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_rx_status_report_test)

add_executable(pdcp_rx_reestablish_test pdcp_rx_reestablish_test.cpp)
target_link_libraries(pdcp_rx_reestablish_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_reestablish_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_rx_reestablish_test pdcp_rx_reestablish_test)
set_tests_properties(pdcp_rx_reestablish_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_rx_reestablish_test)

add_executable(pdcp_tx_reestablish_test pdcp_tx_reestablish_test.cpp)
target_link_libraries(pdcp_tx_reestablish_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_reestablish_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_reestablish_test pdcp_tx_reestablish_test)
set_tests_properties(pdcp_tx_reestablish_test PROPERTIES LABELS "tsan")
gtest_discover_tests(pdcp_tx_reestablish_test)

add_executable(pdcp_rx_stop_test pdcp_rx_stop_test.cpp)
target_link_libraries(pdcp_rx_stop_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_stop_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_rx_stop_test pdcp_rx_stop_test)
gtest_discover_tests(pdcp_rx_stop_test)

add_executable(pdcp_tx_stop_test pdcp_tx_stop_test.cpp)
target_link_libraries(pdcp_tx_stop_test srsran_pdcp srsran_security srsran_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_stop_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(pdcp_tx_stop_test pdcp_tx_stop_test)
gtest_discover_tests(pdcp_tx_stop_test)

add_executable(pdcp_gen_helper pdcp_gen_helper.cpp)
target_link_libraries(pdcp_gen_helper srsran_pdcp srsran_security srsran_support srslog)
target_include_directories(pdcp_gen_helper PRIVATE ${CMAKE_SOURCE_DIR})

#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(pdcp_tx_test pdcp_tx_test.cpp)
target_link_libraries(pdcp_tx_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_test)

add_executable(pdcp_rx_test pdcp_rx_test.cpp)
target_link_libraries(pdcp_rx_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_test)

add_executable(pdcp_tx_metrics_test pdcp_tx_metrics_test.cpp)
target_link_libraries(pdcp_tx_metrics_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_metrics_test)

add_executable(pdcp_rx_metrics_test pdcp_rx_metrics_test.cpp)
target_link_libraries(pdcp_rx_metrics_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_metrics_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_metrics_test)

add_executable(pdcp_tx_status_report_test pdcp_tx_status_report_test.cpp)
target_link_libraries(pdcp_tx_status_report_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_tx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_tx_status_report_test)

add_executable(pdcp_rx_status_report_test pdcp_rx_status_report_test.cpp)
target_link_libraries(pdcp_rx_status_report_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
target_include_directories(pdcp_rx_status_report_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdcp_rx_status_report_test)

add_executable(pdcp_gen_helper pdcp_gen_helper.cpp)
target_link_libraries(pdcp_gen_helper srsgnb_pdcp srsgnb_security srsgnb_support srslog)
target_include_directories(pdcp_gen_helper PRIVATE ${CMAKE_SOURCE_DIR})

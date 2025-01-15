#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "rlc")

add_executable(rlc_rx_am_test rlc_rx_am_test.cpp)
target_link_libraries(rlc_rx_am_test srsran_rlc srsran_support srslog gtest gtest_main pdcp_test_doubles)
target_include_directories(rlc_rx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_am_test)

add_executable(rlc_tx_am_test rlc_tx_am_test.cpp)
target_link_libraries(rlc_tx_am_test srsran_rlc srsran_support srslog gtest gtest_main pdcp_test_doubles)
target_include_directories(rlc_tx_am_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_am_test)

add_executable(rlc_rx_tm_test rlc_rx_tm_test.cpp)
target_link_libraries(rlc_rx_tm_test srsran_rlc srsran_support srslog gtest gtest_main pdcp_test_doubles)
target_include_directories(rlc_rx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_rx_tm_test)

add_executable(rlc_tx_tm_test rlc_tx_tm_test.cpp)
target_link_libraries(rlc_tx_tm_test srsran_rlc srsran_support srslog gtest gtest_main pdcp_test_doubles)
target_include_directories(rlc_tx_tm_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_tx_tm_test)

add_executable(rlc_um_test rlc_um_test.cpp)
target_link_libraries(rlc_um_test srsran_rlc srsran_support srslog gtest gtest_main pdcp_test_doubles)
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
target_link_libraries(rlc_pdu_recycler_test srsran_instrumentation srsran_support srslog gtest gtest_main pdcp_test_doubles)
target_include_directories(rlc_pdu_recycler_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_pdu_recycler_test)

add_executable(rlc_sdu_queue_lockfree_test rlc_sdu_queue_lockfree_test.cpp)
target_link_libraries(rlc_sdu_queue_lockfree_test srsran_support srslog)
target_include_directories(rlc_sdu_queue_lockfree_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_sdu_queue_lockfree_test)

add_executable(rlc_retx_queue_test rlc_retx_queue_test.cpp)
target_link_libraries(rlc_retx_queue_test srsran_support srslog gtest gtest_main)
target_include_directories(rlc_retx_queue_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_retx_queue_test)

add_executable(rlc_metrics_aggregator_test rlc_metrics_aggregator_test.cpp)
target_link_libraries(rlc_metrics_aggregator_test srsran_rlc srsran_support srslog gtest gtest_main)
target_include_directories(rlc_metrics_aggregator_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(rlc_metrics_aggregator_test)

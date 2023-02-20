#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pdcp")

add_executable(sdap_tx_test sdap_tx_test.cpp)
target_link_libraries(sdap_tx_test srsran_support srslog gtest gtest_main)
target_include_directories(sdap_tx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_tx_test)

add_executable(sdap_rx_test sdap_rx_test.cpp)
target_link_libraries(sdap_rx_test srsran_support srslog gtest gtest_main)
target_include_directories(sdap_rx_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sdap_rx_test)

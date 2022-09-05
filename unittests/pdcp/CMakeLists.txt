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
gtest_discover_tests(pdcp_tx_test)

add_executable(pdcp_rx_test pdcp_rx_test.cpp)
target_link_libraries(pdcp_rx_test srsgnb_pdcp srsgnb_security srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(pdcp_rx_test)

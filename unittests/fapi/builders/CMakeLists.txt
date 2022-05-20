#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(crc_indication_builder_test crc_indication_test.cpp)
target_link_libraries(crc_indication_builder_test srsgnb_support srslog)
add_test(crc_indication_builder_test crc_indication_builder_test)

add_executable(dl_pdcch_builder_test pdcch_test.cpp)
target_link_libraries(dl_pdcch_builder_test srsgnb_support srslog)
add_test(dl_pdcch_builder_test dl_pdcch_builder_test)

add_executable(dl_pdsch_builder_test pdsch_test.cpp)
target_link_libraries(dl_pdsch_builder_test srsgnb_support srslog)
add_test(dl_pdsch_builder_test dl_pdsch_builder_test)

add_executable(rach_indicator_builder_test rach_indicator_test.cpp)
target_link_libraries(rach_indicator_builder_test srsgnb_support srslog)
add_test(rach_indicator_builder_test rach_indicator_builder_test)

add_executable(tx_data_request_builder_test tx_data_request_test.cpp)
target_link_libraries(tx_data_request_builder_test srsgnb_support srslog)
add_test(tx_data_request_builder_test tx_data_request_builder_test)

add_executable(ul_dci_request_builder_test ul_dci_request_test.cpp)
target_link_libraries(ul_dci_request_builder_test srsgnb_support srslog)
add_test(ul_dci_request_builder_test ul_dci_request_builder_test)

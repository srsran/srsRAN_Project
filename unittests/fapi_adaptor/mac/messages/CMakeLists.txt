#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_mac_translator_helpers STATIC helpers.cpp)

add_executable(mac_fapi_pdcch_adaptor_test dl_pdcch_pdu_test.cpp)
target_link_libraries(mac_fapi_pdcch_adaptor_test srsgnb_mac_fapi_adaptors srsgnb_support srslog srsgnb_mac_translator_helpers)
add_test(mac_fapi_pdcch_adaptor_test mac_fapi_pdcch_adaptor_test)

add_executable(mac_fapi_ssb_adaptor_test dl_ssb_pdu_test.cpp)
target_link_libraries(mac_fapi_ssb_adaptor_test srsgnb_mac_fapi_adaptors srsgnb_support srslog srsgnb_mac_translator_helpers)
add_test(mac_fapi_ssb_adaptor_test mac_fapi_ssb_adaptor_test)

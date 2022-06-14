#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(mac_fapi_ssb_adaptor_test dl_ssb_pdu_test.cpp)
target_link_libraries(mac_fapi_ssb_adaptor_test srsgnb_mac_fapi_adaptors srsgnb_support srslog)
add_test(mac_fapi_ssb_adaptor_test mac_fapi_ssb_adaptor_test)

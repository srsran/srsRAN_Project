#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_executable(fapi_adaptor_test fapi_adaptor_test.cpp)
target_link_libraries(fapi_adaptor_test srsgnb_fapi_phy_message_adaptors srslog)
add_test(fapi_adaptor_test fapi_adaptor_test)

add_executable(pdcch_adaptor_test pdcch_adaptor_test.cpp)
target_link_libraries(pdcch_adaptor_test srsgnb_fapi_phy_message_adaptors srslog)
add_test(pdcch_adaptor_test pdcch_adaptor_test)

add_executable(pdsch_adaptor_test pdsch_adaptor_test.cpp)
target_link_libraries(pdsch_adaptor_test srsgnb_fapi_phy_message_adaptors srsgnb_upper_phy_support srslog)
add_test(pdsch_adaptor_test pdsch_adaptor_test)

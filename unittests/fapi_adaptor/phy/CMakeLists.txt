#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

add_executable(fapi_to_phy_translator_test fapi_to_phy_translator_test.cpp)
target_link_libraries(fapi_to_phy_translator_test fapi_to_phy_translator srslog srsgnb_fapi_validator_test_helpers)
add_test(fapi_to_phy_translator_test fapi_to_phy_translator_test)

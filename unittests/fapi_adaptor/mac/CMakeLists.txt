#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(messages)

add_executable(mac_to_fapi_translator_test mac_to_fapi_translator_test.cpp)
target_link_libraries(mac_to_fapi_translator_test srsgnb_mac_to_fapi_translator srslog srsgnb_mac_translator_helpers)
add_test(mac_to_fapi_translator_test mac_to_fapi_translator_test)

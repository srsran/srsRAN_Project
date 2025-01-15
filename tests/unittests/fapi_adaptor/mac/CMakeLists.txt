#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(messages)

add_executable(fapi_to_mac_data_msg_translator_test fapi_to_mac_data_msg_translator_test.cpp)
target_link_libraries(fapi_to_mac_data_msg_translator_test
        srsran_support
        srsran_fapi_to_mac_translator
        srslog
        srsran_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(fapi_to_mac_data_msg_translator_test)

add_executable(mac_to_fapi_translator_test mac_to_fapi_translator_test.cpp)
target_link_libraries(mac_to_fapi_translator_test
        srsran_support
        srsran_mac_to_fapi_translator
        srsran_fapi_precoding_matrix_tools
        srslog
        srsran_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(mac_to_fapi_translator_test)

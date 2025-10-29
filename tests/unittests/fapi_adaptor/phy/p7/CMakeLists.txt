#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

add_executable(fapi_to_phy_fastpath_translator_test fapi_to_phy_fastpath_translator_test.cpp)
target_include_directories(fapi_to_phy_fastpath_translator_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/phy/p7)
target_link_libraries(fapi_to_phy_fastpath_translator_test PRIVATE srsran_fapi_to_phy_p7_fastpath_translator
                                                                   srsran_fapi_precoding_matrix_tools
                                                                   srsran_fapi_uci_part2_tools
                                                                   srslog
                                                                   srsran_fapi_message_builder_test_helpers
                                                                   srsran_upper_phy_support gtest gtest_main)
gtest_discover_tests(fapi_to_phy_fastpath_translator_test)

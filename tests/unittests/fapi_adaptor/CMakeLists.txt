#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(mac/p7)
add_subdirectory(phy/p7)

add_executable(precoding_matrix_table_generator_test precoding_matrix_table_generator_test.cpp)
target_link_libraries(precoding_matrix_table_generator_test srsran_fapi_precoding_matrix_tools srslog gtest gtest_main)
gtest_discover_tests(precoding_matrix_table_generator_test)

add_executable(uci_part2_correspondence_test uci_part2_correspondence_test.cpp)
target_link_libraries(uci_part2_correspondence_test srsran_fapi_uci_part2_tools srslog gtest gtest_main)
gtest_discover_tests(uci_part2_correspondence_test)

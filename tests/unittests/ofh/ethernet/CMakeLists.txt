#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ethernet")

add_executable(vlan_ethernet_frame_builder_test vlan_ethernet_frame_builder_test.cpp)
target_link_libraries(vlan_ethernet_frame_builder_test srsran_ofh_ethernet srsran_support srslog gtest gtest_main)
gtest_discover_tests(vlan_ethernet_frame_builder_test)

add_executable(vlan_ethernet_frame_decoder_test vlan_ethernet_frame_decoder_test.cpp)
target_link_libraries(vlan_ethernet_frame_decoder_test srsran_ofh_ethernet srsran_support srslog gtest gtest_main)
gtest_discover_tests(vlan_ethernet_frame_decoder_test)

add_executable(ethernet_frame_pool_test ethernet_frame_pool_test.cpp)
target_link_libraries(ethernet_frame_pool_test srsran_ofh_ethernet srslog gtest gtest_main)
gtest_discover_tests(ethernet_frame_pool_test)

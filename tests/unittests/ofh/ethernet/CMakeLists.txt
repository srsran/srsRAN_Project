#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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

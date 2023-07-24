#
# Copyright 2021-2023 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "transmitter")

add_executable(ofh_data_flow_cplane_scheduling_commands_test ofh_data_flow_cplane_scheduling_commands_test.cpp)
target_link_libraries(ofh_data_flow_cplane_scheduling_commands_test srsran_ofh_transmitter srsran_support gtest gtest_main)
gtest_discover_tests(ofh_data_flow_cplane_scheduling_commands_test)

add_executable(ofh_data_flow_uplane_downlink_data_impl_test ofh_data_flow_uplane_downlink_data_impl_test.cpp)
target_link_libraries(ofh_data_flow_uplane_downlink_data_impl_test srsran_ofh_transmitter srsran_support gtest gtest_main)
gtest_discover_tests(ofh_data_flow_uplane_downlink_data_impl_test)

add_executable(ofh_downlink_handler_impl_test ofh_downlink_handler_impl_test.cpp)
target_link_libraries(ofh_downlink_handler_impl_test srsran_ofh_transmitter srsran_support gtest gtest_main)
gtest_discover_tests(ofh_downlink_handler_impl_test)

add_executable(ofh_uplane_packet_segment_calculator_test ofh_uplane_packet_segment_calculator_test.cpp)
target_link_libraries(ofh_uplane_packet_segment_calculator_test srsran_ofh_transmitter srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_packet_segment_calculator_test)

add_executable(ofh_uplink_request_handler_impl_test ofh_uplink_request_handler_impl_test.cpp)
target_link_libraries(ofh_uplink_request_handler_impl_test srsran_ofh_transmitter srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplink_request_handler_impl_test)

add_executable(sequence_identifier_generator_test sequence_identifier_generator_test.cpp)
target_link_libraries(sequence_identifier_generator_test srsran_support gtest gtest_main)
gtest_discover_tests(sequence_identifier_generator_test)

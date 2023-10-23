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

set_directory_properties(PROPERTIES LABELS "receiver")

add_executable(ofh_data_flow_uplane_uplink_data_impl_test ofh_data_flow_uplane_uplink_data_impl_test.cpp)
target_link_libraries(ofh_data_flow_uplane_uplink_data_impl_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_data_flow_uplane_uplink_data_impl_test)

add_executable(ofh_data_flow_uplane_uplink_prach_impl_test ofh_data_flow_uplane_uplink_prach_impl_test.cpp)
target_link_libraries(ofh_data_flow_uplane_uplink_prach_impl_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_data_flow_uplane_uplink_prach_impl_test)

add_executable(ofh_message_receiver_test ofh_message_receiver_test.cpp)
target_link_libraries(ofh_message_receiver_test srsran_ofh srsran_support gtest gtest_main)
gtest_discover_tests(ofh_message_receiver_test)

add_executable(ofh_rx_window_checker_test ofh_rx_window_checker_test.cpp)
target_link_libraries(ofh_rx_window_checker_test srsran_ofh srsran_support gtest gtest_main)
gtest_discover_tests(ofh_rx_window_checker_test)

add_executable(ofh_uplane_prach_data_flow_notifier_test ofh_uplane_prach_data_flow_notifier_test.cpp)
target_link_libraries(ofh_uplane_prach_data_flow_notifier_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_prach_data_flow_notifier_test)

add_executable(ofh_uplane_prach_symbol_data_flow_writer_test ofh_uplane_prach_symbol_data_flow_writer_test.cpp)
target_link_libraries(ofh_uplane_prach_symbol_data_flow_writer_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_prach_symbol_data_flow_writer_test)

add_executable(ofh_uplane_rx_symbol_data_flow_notifier_test ofh_uplane_rx_symbol_data_flow_notifier_test.cpp)
target_link_libraries(ofh_uplane_rx_symbol_data_flow_notifier_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_rx_symbol_data_flow_notifier_test)

add_executable(ofh_uplane_rx_symbol_data_flow_writer_test ofh_uplane_rx_symbol_data_flow_writer_test.cpp)
target_link_libraries(ofh_uplane_rx_symbol_data_flow_writer_test srsran_ofh_receiver srsran_support gtest gtest_main)
gtest_discover_tests(ofh_uplane_rx_symbol_data_flow_writer_test)

add_executable(ofh_sequence_id_checker_test ofh_sequence_id_checker_test.cpp)
target_link_libraries(ofh_sequence_id_checker_test srsran_support gtest gtest_main)
gtest_discover_tests(ofh_sequence_id_checker_test)


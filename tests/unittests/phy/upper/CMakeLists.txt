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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(equalization)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(downlink_processor_pool_test downlink_processor_pool_test.cpp)
target_link_libraries(downlink_processor_pool_test srsran_upper_phy srslog)
add_test(downlink_processor_pool_test downlink_processor_pool_test)

add_executable(downlink_processor_test downlink_processor_test.cpp)
target_link_libraries(downlink_processor_test srsran_downlink_processor srslog gtest gtest_main)
gtest_discover_tests(downlink_processor_test)

add_executable(hard_decision_test hard_decision_test.cpp)
target_link_libraries(hard_decision_test log_likelihood_ratio srslog gtest gtest_main)
gtest_discover_tests(hard_decision_test)

add_executable(log_likelihood_ratio_test log_likelihood_ratio_test.cpp)
target_link_libraries(log_likelihood_ratio_test log_likelihood_ratio srslog)
add_test(log_likelihood_ratio_test log_likelihood_ratio_test)

add_executable(rx_softbuffer_pool_test rx_softbuffer_pool_test.cpp)
target_link_libraries(rx_softbuffer_pool_test srsran_upper_phy_support srslog)
add_test(rx_softbuffer_pool_test rx_softbuffer_pool_test)

add_executable(uplink_processor_test uplink_processor_test.cpp)
target_link_libraries(uplink_processor_test srsran_uplink_processor srslog gtest gtest_main)
gtest_discover_tests(uplink_processor_test)

add_executable(uplink_request_processor_test uplink_request_processor_test.cpp)
target_link_libraries(uplink_request_processor_test srsran_uplink_processor srsran_phy_support srslog gtest gtest_main)
gtest_discover_tests(uplink_request_processor_test)

add_executable(upper_phy_rx_symbol_handler_test upper_phy_rx_symbol_handler_test.cpp)
target_link_libraries(upper_phy_rx_symbol_handler_test srsran_upper_phy srsran_phy_support srslog gtest gtest_main)
gtest_discover_tests(upper_phy_rx_symbol_handler_test)

add_executable(vrb_to_prb_mapper_test vrb_to_prb_mapper_test.cpp)
target_link_libraries(vrb_to_prb_mapper_test srsran_upper_phy_support srslog)
add_test(vrb_to_prb_mapper_test vrb_to_prb_mapper_test)

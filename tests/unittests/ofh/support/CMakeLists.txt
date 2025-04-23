#
# Copyright 2021-2025 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "support")

add_executable(context_repository_helpers_test context_repository_helpers_test.cpp)
target_link_libraries(context_repository_helpers_test srsran_support gtest gtest_main)
target_include_directories(context_repository_helpers_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/ofh/support)
gtest_discover_tests(context_repository_helpers_test)

add_executable(uplink_notified_grid_symbol_repository_test uplink_notified_grid_symbol_repository_test.cpp)
target_link_libraries(uplink_notified_grid_symbol_repository_test srsran_support gtest gtest_main)
target_include_directories(uplink_notified_grid_symbol_repository_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/ofh/support)
gtest_discover_tests(uplink_notified_grid_symbol_repository_test)

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

add_library(cell_meas_manager_helpers cell_meas_manager_test_helpers.cpp)
target_include_directories(cell_meas_manager_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_helpers srsran_cu_cp_cell_meas_manager srsran_support srslog gtest gtest_main)

set(SOURCES
        cell_meas_manager_test.cpp)

add_executable(cell_meas_manager_test ${SOURCES})
target_include_directories(cell_meas_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_test cell_meas_manager_helpers)
gtest_discover_tests(cell_meas_manager_test PROPERTIES "LABELS;cell_meas_manager")

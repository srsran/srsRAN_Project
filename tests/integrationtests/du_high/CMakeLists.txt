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

set_directory_properties(PROPERTIES LABELS "du_high;integrationtest")

add_library(du_high_test_bench test_utils/du_high_test_bench.cpp)
target_link_libraries(du_high_test_bench srsran_du_high mac_test_doubles f1ap_test_doubles)
target_include_directories(du_high_test_bench PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(du_high_test du_high_test.cpp du_high_many_ues_test.cpp)
target_link_libraries(du_high_test
        du_high_test_bench
        f1ap_du_test_helpers
        srsran_gateway
        gtest
        gtest_main)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(du_high_test du_high_test)
set_tests_properties(du_high_test PROPERTIES LABELS "tsan")

add_executable(paging_du_high_test paging_test.cpp)
target_link_libraries(paging_du_high_test du_high_test_bench f1ap_du_test_helpers srsran_gateway gtest gtest_main)
target_include_directories(paging_du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(paging_du_high_test paging_du_high_test)
set_tests_properties(paging_du_high_test PROPERTIES LABELS "tsan")

add_executable(mac_test_mode_adapter_test mac_test_mode_adapter_test.cpp)
target_link_libraries(mac_test_mode_adapter_test srsran_du_high gtest gtest_main)
target_include_directories(mac_test_mode_adapter_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(mac_test_mode_adapter_test mac_test_mode_adapter_test)

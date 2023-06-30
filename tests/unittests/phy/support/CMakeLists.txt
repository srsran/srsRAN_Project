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

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(resource_grid_pool_test resource_grid_pool_test.cpp)
target_link_libraries(resource_grid_pool_test srsvec srsran_phy_support srslog)
add_test(resource_grid_pool_test resource_grid_pool_test)

add_executable(resource_grid_test resource_grid_test.cpp)
target_link_libraries(resource_grid_test srsvec srsran_phy_support srsran_channel_precoder srslog)
add_test(resource_grid_test resource_grid_test)

add_executable(interpolator_test interpolator_test.cpp)
target_link_libraries(interpolator_test srsran_phy_support srslog gtest gtest_main)
add_test(interpolator_test interpolator_test)

add_executable(resource_grid_mapper_test resource_grid_mapper_test.cpp)
target_link_libraries(resource_grid_mapper_test
        srsvec
        srsran_phy_support
        srsran_upper_phy_support
        srsran_channel_precoder
        srsran_ran
        srslog
        gtest
        gtest_main)
gtest_discover_tests(resource_grid_mapper_test)

add_executable(precoding_configuration_test precoding_configuration_test.cpp)
target_link_libraries(precoding_configuration_test
        srslog
        gtest
        gtest_main)
gtest_discover_tests(precoding_configuration_test)

add_library(resource_grid_mapper_test_doubles STATIC resource_grid_mapper_test_doubles.cpp)
target_link_libraries(srsran_upper_phy_support)

add_executable(re_pattern_test re_pattern_test.cpp)
target_link_libraries(re_pattern_test srsran_phy_support srslog)
add_test(re_pattern_test re_pattern_test)


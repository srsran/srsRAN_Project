#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

add_executable(re_pattern_test re_pattern_test.cpp)
target_link_libraries(re_pattern_test srsran_phy_support srslog)
add_test(re_pattern_test re_pattern_test)


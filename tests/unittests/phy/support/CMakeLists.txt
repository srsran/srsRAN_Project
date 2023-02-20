#
# Copyright 2021-2023 Software Radio Systems Limited
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
target_link_libraries(resource_grid_test srsvec srsran_phy_support srslog)
add_test(resource_grid_test resource_grid_test)

add_executable(interpolator_test interpolator_test.cpp)
target_link_libraries(interpolator_test srsran_phy_support srslog gtest gtest_main)
add_test(interpolator_test interpolator_test)


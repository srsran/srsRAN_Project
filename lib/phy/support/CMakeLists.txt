#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_phy_support STATIC
        interpolator/interpolator_linear_impl.cpp
        prach_buffer_pool_impl.cpp
        resource_grid_pool_impl.cpp
        resource_grid_impl.cpp
        support_factories.cpp)
target_link_libraries(srsran_phy_support srsvec)
#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(generic_functions)
add_subdirectory(lower)
add_subdirectory(upper)

add_library(resource_grid_pool STATIC resource_grid_pool_impl.cpp resource_grid_impl.cpp)
target_link_libraries(resource_grid_pool srsvec)

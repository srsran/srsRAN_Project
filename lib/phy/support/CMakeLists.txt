#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_phy_support STATIC
        interpolator/interpolator_linear_impl.cpp
        time_alignment_estimator/time_alignment_estimator_dft_impl.cpp
        time_alignment_estimator/time_alignment_estimator_factories.cpp
        prach_buffer_pool_impl.cpp
        re_pattern.cpp
        resource_grid_impl.cpp
        resource_grid_mapper_impl.cpp
        resource_grid_pool_asynchronous_impl.cpp
        resource_grid_pool_generic_impl.cpp
        resource_grid_reader_impl.cpp
        resource_grid_writer_impl.cpp
        support_factories.cpp)
target_link_libraries(srsran_phy_support srsvec srsran_instrumentation)

add_to_exported_libs(srsran_phy_support)

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

add_library(srsran_phy_support STATIC
        interpolator/interpolator_linear_impl.cpp
        time_alignment_estimator/time_alignment_estimator_dft_impl.cpp
        time_alignment_estimator/time_alignment_estimator_factories.cpp
        prach_buffer_pool_impl.cpp
        rb_allocation.cpp
        re_pattern.cpp
        resource_grid_impl.cpp
        resource_grid_mapper_impl.cpp
        resource_grid_pool_impl.cpp
        resource_grid_reader_impl.cpp
        resource_grid_writer_impl.cpp
        support_factories.cpp)
target_link_libraries(srsran_phy_support srsvec srsran_instrumentation srsran_ran)

add_to_exported_libs(srsran_phy_support)

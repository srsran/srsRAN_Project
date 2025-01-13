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

add_subdirectory(du_high)
add_subdirectory(e2)
add_subdirectory(fapi)

add_library(srsran_o_du_high_unit_helpers STATIC o_du_high_unit_factory.cpp)
target_include_directories(srsran_o_du_high_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_o_du_high_unit_helpers PRIVATE
        srsran_du_high_unit_helpers
        srsran_fapi_app_unit
        srsran_o_du_high_app_unit_e2
        srsran_o_du_high)

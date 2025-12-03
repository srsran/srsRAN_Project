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

set(SOURCES
        configuration_procedure.cpp
        phy_fapi_p5_sector_operation_request_adaptor.cpp)

add_library(srsran_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor STATIC ${SOURCES})
target_include_directories(srsran_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor PRIVATE ${CMAKE_SOURCE_DIR})

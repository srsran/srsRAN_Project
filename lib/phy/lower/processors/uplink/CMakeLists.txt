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

add_subdirectory(prach)
add_subdirectory(puxch)

add_library(srsran_lower_phy_uplink_processor STATIC
        uplink_processor_factories.cpp
        uplink_processor_impl.cpp)
target_link_libraries(srsran_lower_phy_uplink_processor
        srsran_lower_phy_prach_processor
        srsran_lower_phy_puxch_processor)
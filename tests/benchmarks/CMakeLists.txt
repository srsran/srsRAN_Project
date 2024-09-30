#
# Copyright 2021-2024 Software Radio Systems Limited
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

remove_definitions(-DASSERTS_ENABLED)
remove_definitions(-DPARANOID_ASSERTS_ENABLED)

add_subdirectory(adt)
add_subdirectory(gateways)
if (NOT DU_SPLIT_6)
    add_subdirectory(phy)
endif ()
add_subdirectory(scheduler)
add_subdirectory(du_high)
if (DU_SPLIT_7_2 OR DU_SPLIT_DYNAMIC)
    add_subdirectory(ofh)
endif ()
add_subdirectory(rlc)
add_subdirectory(pdcp)

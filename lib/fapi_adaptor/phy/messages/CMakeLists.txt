#
# Copyright 2021-2023 Software Radio Systems Limited
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
        csi_rs.cpp
        pdcch.cpp
        pdsch.cpp
        prach.cpp
        pucch.cpp
        pusch.cpp
        ssb.cpp)

add_library(srsran_fapi_phy_message_adaptors STATIC ${SOURCES})
target_link_libraries(srsran_fapi_phy_message_adaptors srsvec srsran_phy_support srsran_fapi_precoding_matrix_tools srsran_upper_phy_support srsran_ran)

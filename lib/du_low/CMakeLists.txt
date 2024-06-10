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

set(SOURCES
        du_low_factory.cpp
        du_low_impl.cpp
        du_low_wrapper_factory.cpp
        du_low_wrapper_impl.cpp)

add_library(srsran_du_low_wrapper STATIC ${SOURCES})
target_link_libraries(srsran_du_low_wrapper srsran_phy_fapi_adaptor srsran_upper_phy srsran_channel_precoder)

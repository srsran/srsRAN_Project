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

if (ZEROMQ_FOUND OR UHD_FOUND)
        add_executable(du_example
                du_example.cpp
                fapi_factory.cpp
                phy_factory.cpp)

        target_link_libraries(du_example
                srsran_du_high
                srsran_gateway
                mac_fapi_adaptor_factory
                phy_fapi_adaptor_factory
                srsran_fapi
                srsran_phy_support
                srsran_upper_phy
                srsran_signal_processors
                srsran_channel_processors
                srsran_channel_equalizer
                srsran_channel_precoder
                srsran_ru_generic)

        target_include_directories(du_example PRIVATE ${CMAKE_SOURCE_DIR})
endif (ZEROMQ_FOUND OR UHD_FOUND)

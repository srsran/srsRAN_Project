#
# Copyright 2021-2026 Software Radio Systems Limited
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
        lower_phy/lower_phy_sector.cpp
        lower_phy/lower_phy_sector_metrics_collector.cpp
        lower_phy/lower_phy_sector_timing_handler.cpp
        lower_phy/lower_phy_factory.cpp
        lower_phy/lower_phy_sector.cpp
        ru_controller_sdr_impl.cpp
        ru_factory_sdr_impl.cpp
        ru_lower_phy_downlink_handler_impl.cpp
        ru_lower_phy_uplink_request_handler_impl.cpp
        ru_metrics_collector_sdr_impl.cpp
        ru_radio_metrics_collector.cpp
        ru_sdr_executor_mapper.cpp
        ru_sdr_impl.cpp)

add_library(srsran_ru_sdr STATIC ${SOURCES})
target_link_libraries(srsran_ru_sdr srsran_lower_phy srsran_radio srslog)

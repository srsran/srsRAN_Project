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

add_subdirectory(modulation)
add_subdirectory(processors)
add_subdirectory(amplitude_controller)

add_library(srsran_lower_phy STATIC
        lower_phy_baseband_processor.cpp
        lower_phy_factory.cpp
        lower_phy_impl.cpp)
target_link_libraries(srsran_lower_phy
        srsran_generic_funcs
        srsran_lower_phy_adaptors
        srsran_lower_phy_amplitude_controller
        srsran_lower_phy_downlink_processor
        srsran_lower_phy_pdxch_processor
        srsran_lower_phy_modulation
        srsran_lower_phy_prach_processor
        srsran_lower_phy_puxch_processor
        srsran_lower_phy_uplink_processor)

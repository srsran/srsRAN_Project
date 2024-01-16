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
        ethernet_factories.cpp
        ethernet_transmitter_impl.cpp
        ethernet_receiver_impl.cpp
        vlan_ethernet_frame_builder_impl.cpp
        vlan_ethernet_frame_decoder_impl.cpp)

add_library(srsran_ofh_ethernet STATIC ${SOURCES})
target_link_libraries(srsran_ofh_ethernet srslog)

if (DPDK_FOUND)
    add_subdirectory(dpdk)
    target_link_libraries(srsran_ofh_ethernet srsran_ofh_dpdk_ethernet)
endif (DPDK_FOUND)

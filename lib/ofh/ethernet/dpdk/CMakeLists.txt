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
        dpdk_ethernet_transmitter.cpp
        dpdk_ethernet_factories.cpp
        dpdk_ethernet_receiver.cpp)

add_library(srsran_ofh_dpdk_ethernet STATIC ${SOURCES})
target_compile_options(srsran_ofh_dpdk_ethernet PRIVATE ${DPDK_CFLAGS})
target_link_libraries(srsran_ofh_dpdk_ethernet srslog ${DPDK_LIBRARIES})

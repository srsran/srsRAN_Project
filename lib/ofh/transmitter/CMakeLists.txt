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
        ofh_data_flow_cplane_scheduling_commands_impl.cpp
        ofh_data_flow_uplane_downlink_data_impl.cpp
        ofh_downlink_handler_broadcast_impl.cpp
        ofh_downlink_handler_impl.cpp
        ofh_downlink_manager_broadcast_impl.cpp
        ofh_downlink_manager_impl.cpp
        ofh_message_transmitter_impl.cpp
        ofh_uplane_fragment_size_calculator.cpp
        ofh_uplink_request_handler_impl.cpp
        ofh_transmitter_factories.cpp
        ofh_transmitter_impl.cpp)

add_library(srsran_ofh_transmitter STATIC ${SOURCES})
target_link_libraries(srsran_ofh_transmitter
        srsran_ran
        srsran_instrumentation
        srslog)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)

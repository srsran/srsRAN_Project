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
        ofh_data_flow_uplane_uplink_data_impl.cpp
        ofh_data_flow_uplane_uplink_prach_impl.cpp
        ofh_message_receiver.cpp
        ofh_receiver_factories.cpp
        ofh_receiver_impl.cpp
        ofh_rx_window_checker.cpp
        ofh_uplane_rx_symbol_data_flow_notifier.cpp
        ofh_uplane_rx_symbol_data_flow_writer.cpp
        ofh_uplane_prach_data_flow_notifier.cpp
        ofh_uplane_prach_symbol_data_flow_writer.cpp)

add_library(srsran_ofh_receiver STATIC ${SOURCES})
target_link_libraries(srsran_ofh_receiver srsran_ran srsran_ofh_message_serdes srsran_ofh_compression srsran_ofh_ethernet srsran_ofh_ecpri srslog)

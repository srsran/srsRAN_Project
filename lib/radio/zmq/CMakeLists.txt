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

set(SOURCES_ZMQ
        radio_config_zmq_validator.cpp
        radio_factory_zmq_impl.cpp
        radio_session_zmq_impl.cpp
        radio_zmq_rx_channel.cpp
        radio_zmq_rx_stream.cpp
        radio_zmq_tx_channel.cpp
        radio_zmq_tx_stream.cpp)
add_library(srsran_radio_zmq STATIC ${SOURCES_ZMQ})

target_link_libraries(srsran_radio_zmq ${ZEROMQ_LIBRARIES} fmt srslog)

install(TARGETS srsran_radio_zmq EXPORT srsran_export)
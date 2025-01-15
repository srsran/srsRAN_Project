#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

add_to_exported_libs(srsran_radio_zmq)

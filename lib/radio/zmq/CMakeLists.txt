#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES_ZMQ
        radio_factory_zmq_impl.cpp
        radio_session_zmq_impl.cpp
        radio_zmq_rx_channel.cpp
        radio_zmq_rx_stream.cpp
        radio_zmq_tx_channel.cpp
        radio_zmq_tx_stream.cpp)
add_library(srsgnb_radio_zmq STATIC ${SOURCES_ZMQ})

target_link_libraries(srsgnb_radio_zmq ${ZEROMQ_LIBRARIES})

install(TARGETS srsgnb_radio_zmq EXPORT srsgnb_export)
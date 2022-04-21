
set(SOURCES_ZMQ
        radio_factory_zmq_impl.cpp
        radio_session_zmq_impl.cpp
        radio_zmq_rx_channel.cpp
        radio_zmq_rx_stream.cpp
        radio_zmq_tx_channel.cpp
        radio_zmq_tx_stream.cpp)
add_library(srsgnb_radio_zmq STATIC ${SOURCES_ZMQ})

target_link_libraries(srsgnb_radio_zmq ${ZEROMQ_LIBRARIES})
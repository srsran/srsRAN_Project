
set(SOURCES_UHD radio_uhd_impl.cpp radio_uhd_rx_stream.cpp radio_uhd_tx_stream.cpp)
add_library(srsgnb_radio_uhd STATIC ${SOURCES_UHD})

target_link_libraries(srsgnb_radio_uhd ${UHD_LIBRARIES} ${Boost_LIBRARIES})
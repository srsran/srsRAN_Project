#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES_UHD
        radio_config_uhd_validator.cpp
        radio_uhd_impl.cpp
        radio_uhd_rx_stream.cpp
        radio_uhd_tx_stream.cpp)
add_library(srsgnb_radio_uhd STATIC ${SOURCES_UHD})

target_link_libraries(srsgnb_radio_uhd ${UHD_LIBRARIES})

install(TARGETS srsgnb_radio_uhd EXPORT srsgnb_export)
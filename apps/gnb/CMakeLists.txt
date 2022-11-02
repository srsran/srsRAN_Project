#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(gnb
        gnb.cpp
        fapi_factory.cpp
        phy_factory.cpp
        radio_factory.cpp)

target_link_libraries(gnb
        srsgnb_cu_cp
        srsgnb_io_broker
        srsgnb_ngap
        ngap_asn1 
        srsgnb_du_high
        srsgnb_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsgnb_phy_support
        srsgnb_upper_phy
        srsgnb_signal_processors
        srsgnb_channel_processors
        srsgnb_channel_equalizer
        srsgnb_lower_phy
        srsgnb_radio
)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)

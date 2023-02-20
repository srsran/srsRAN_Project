#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(adapters)

add_executable(gnb
        gnb.cpp
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp        
        fapi_factory.cpp
        phy_factory.cpp
        helpers/metrics_plotter_stdout.cpp
        helpers/gnb_console_helper.cpp
        )

target_link_libraries(gnb
        srsgnbapp_f1u_connector
        srsgnb_cu_up
        srsgnb_cu_cp
        srsgnb_io_broker
        srsgnb_ngap
        ngap_asn1
        srsgnb_du_high
        srsgnb_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsgnb_fapi_message_loggers
        srsgnb_phy_support
        srsgnb_upper_phy
        srsgnb_signal_processors
        srsgnb_channel_processors
        srsgnb_channel_equalizer
        srsgnb_lower_phy
        srsgnb_radio
        srsgnb_pcap
        srsgnb_support
        srsgnb_build_info
        )
add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)

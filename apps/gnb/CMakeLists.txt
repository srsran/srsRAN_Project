#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(gnb
        gnb.cpp
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp
        gnb_worker_manager.cpp
        gnb_e2_metric_connector_manager.cpp
        gnb_du_factory.cpp
        helpers/metrics_plotter_stdout.cpp
        helpers/metrics_plotter_json.cpp
        helpers/rlc_metrics_plotter_json.cpp
        helpers/gnb_console_helper.cpp
        helpers/metrics_hub.cpp
        adapters/e1ap_gateway_local_connector.cpp
        adapters/f1c_gateway_local_connector.cpp
        adapters/e2_gateway_remote_connector.cpp
        adapters/ngap_adapter.cpp
        )

install(TARGETS gnb
        RUNTIME)

target_link_libraries(gnb
        srsgnb_app_f1u_connector
        srsran_cu_up
        srsran_cu_cp
        srsran_network
        srsran_ngap
        ngap_asn1
        srsran_e2
        e2ap_asn1
        srsran_du_high
        srsran_du
        srsran_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsran_fapi
        srsran_phy_support
        srsran_lower_phy
        srsran_upper_phy
        srsran_signal_processors
        srsran_channel_processors
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_ru_dummy
        srsran_ru_generic
        srsran_ru_ofh
        srsran_pcap
        srsran_support
        srsran_version
        srsran_build_info
        )

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb hal_dpdk)
endif (DPDK_FOUND)

add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)

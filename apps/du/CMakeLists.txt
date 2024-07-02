#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srsdu
    du.cpp
    du_appconfig_cli11_schema.cpp
    du_appconfig_validators.cpp
    du_appconfig_translators.cpp
    du_appconfig_yaml_writer.cpp
    ../gnb/adapters/e2_gateway_remote_connector.cpp # TODO: Delete
)

install(TARGETS srsdu
        RUNTIME)

target_link_libraries(srsdu
        srsran_app_services
        srsgnb_app_f1u_du_split_connector
        srsran_cu_cp
        srsran_network
        srsran_e2
        e2ap_asn1
        srsran_pcap
        srsran_support
        srsran_version
        srsran_build_info
        srsran_flexible_du_dynamic
        srsran_f1c_gateway
        srsran_cu_up # TODO: Delete
)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(srsdu hal_dpdk)
endif (DPDK_FOUND)

add_backward(srsdu)

target_include_directories(srsdu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

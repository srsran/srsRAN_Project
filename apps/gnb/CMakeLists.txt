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
        gnb_appconfig_yaml_writer.cpp
        )

install(TARGETS gnb
        RUNTIME)

target_link_libraries(gnb
        srsran_app_services
        srsgnb_app_f1u_connector
        srsran_cu_cp
        srsran_network
        srsran_ngap
        ngap_asn1
        srsran_e2
        e2ap_asn1
        srsran_gateway
        srsran_pcap
        srsran_support
        srsran_versioning
        srsran_flexible_du
        srsran_f1c_gateway
        srsran_e1_gateway
        srsran_cu_cp_app_unit
        srsran_cu_up_app_unit
        )

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb hal_dpdk)
    if (ENABLE_PUSCH_HWACC)
      add_definitions(-DENABLE_PUSCH_HWACC)
      target_link_libraries(gnb hal_hwacc_pusch hal_bbdev_factory)
    endif (ENABLE_PUSCH_HWACC)
    if (ENABLE_PDSCH_HWACC)
      add_definitions(-DENABLE_PDSCH_HWACC)
      target_link_libraries(gnb hal_hwacc_pdsch hal_bbdev_factory)
    endif (ENABLE_PDSCH_HWACC)
endif (DPDK_FOUND)

add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

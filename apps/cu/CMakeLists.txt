#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu
        cu.cpp
	cu_appconfig_cli11_schema.cpp
	cu_worker_manager.cpp
	../gnb/adapters/e1ap_gateway_local_connector.cpp
	../gnb/adapters/ngap_adapter.cpp
        )

install(TARGETS cu
        RUNTIME)

target_link_libraries(cu
        srsran_app_services
        srsran_support
        srsran_version
        srsran_build_info
        srsran_cu_cp_app_unit
        srsran_cu_up_app_unit
        srsran_ngap
        srsran_f1c_gateway
        srsgnb_app_f1u_cu_up_split_connector
        srsran_pcap
        ngap_asn1
        )

add_backward(cu)

target_include_directories(cu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)


#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srscu
    cu.cpp
	cu_appconfig_cli11_schema.cpp
	cu_appconfig_validator.cpp
	cu_appconfig_yaml_writer.cpp
	cu_worker_manager.cpp
)

install(TARGETS srscu
        RUNTIME)

target_link_libraries(srscu
        srsran_app_services
        srsran_support
        srsran_versioning
        srsran_cu_cp_app_unit
        srsran_cu_up_app_unit
        srsran_ngap
        srsran_f1c_gateway
		srsran_e1_gateway
        srsgnb_app_f1u_cu_up_split_connector
        srsran_pcap
        ngap_asn1
        )

add_backward(srscu)

target_include_directories(srscu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)


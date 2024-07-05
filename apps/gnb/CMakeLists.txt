#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_executable(gnb
        gnb.cpp
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp
        gnb_appconfig_yaml_writer.cpp
        adapters/e2_gateway_remote_connector.cpp
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
        srsran_version
        srsran_build_info
        srsran_flexible_du_dynamic
        srsran_f1c_gateway
        srsran_e1_gateway
        srsran_cu_cp_app_unit
        srsran_cu_up_app_unit
        )

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb hal_dpdk)
endif (DPDK_FOUND)

add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

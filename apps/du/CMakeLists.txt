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

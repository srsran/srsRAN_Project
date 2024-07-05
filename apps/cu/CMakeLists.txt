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
        srsran_version
        srsran_build_info
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


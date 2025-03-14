#
# Copyright 2021-2025 Software Radio Systems Limited
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
)

install(TARGETS srscu
        RUNTIME)

target_link_libraries(srscu PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_e1_gateway
        srsran_e2
        srsran_f1c_gateway
        srsran_f1u_cu_up_split_connector
        srsran_ngap
        srsran_o_cu_cp_app_unit
        srsran_o_cu_up_app_unit
        srsran_pcap
        srsran_support
        srsran_versioning
        ngap_asn1
)

add_backward(srscu)

target_include_directories(srscu PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
notify_binary_target(srscu)

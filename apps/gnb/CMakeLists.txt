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

set(SOURCES
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp
        gnb_appconfig_yaml_writer.cpp)

add_library(gnb_base STATIC ${SOURCES})
target_include_directories(gnb_base PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(gnb_base PRIVATE
        srsran_app_services
        srsran_f1u_connector
        srsran_ngap
        ngap_asn1
        srsran_e2
        e2ap_asn1
        srsran_gateway
        srsran_pcap
        srsran_support
        srsran_versioning
        srsran_f1c_gateway
        srsran_e1_gateway
        srsran_o_cu_cp_app_unit
        srsran_o_cu_up_app_unit
)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)

target_include_directories(gnb_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

# Default gNB target with dynamic split.
add_executable(gnb gnb.cpp)
add_backward(gnb)
target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS gnb RUNTIME)
target_link_libraries(gnb PRIVATE gnb_base srsran_flexible_o_du_split_dynamic)
notify_binary_target(gnb)

# Add the targets for the rest of the splits.
set(EXTENSIONS gnb_split_6 gnb_split_7_2 gnb_split_8)
set(LIBRARIES srsran_flexible_o_du_split_6 srsran_flexible_o_du_split_7_2 srsran_flexible_o_du_split_8)

foreach (NAME LIB IN ZIP_LISTS EXTENSIONS LIBRARIES)
    add_executable(${NAME} gnb.cpp)
    add_backward(${NAME})
    target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${NAME} PRIVATE gnb_base ${LIB})
    set_target_properties(${NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../${NAME}"   # Path to the binary.
            RUNTIME_OUTPUT_NAME "gnb"                                           # Binary name.
            EXCLUDE_FROM_ALL 1)                                                 # Do not build by default.
    notify_binary_target(${NAME})
endforeach ()

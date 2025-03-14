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
        du_appconfig_cli11_schema.cpp
        du_appconfig_validators.cpp
        du_appconfig_translators.cpp
        du_appconfig_yaml_writer.cpp)


add_library(srsdu_base STATIC ${SOURCES})
target_link_libraries(srsdu_base PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_f1u_du_split_connector
        srsran_cu_cp
        srsran_network
        srsran_e2
        e2ap_asn1
        srsran_pcap
        srsran_support
        srsran_versioning
        srsran_f1c_gateway)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(srsdu_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)

target_include_directories(srsdu_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

# Default DU target with dynamic split.
add_executable(srsdu du.cpp)
add_backward(srsdu)
target_include_directories(srsdu PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS srsdu RUNTIME)
target_link_libraries(srsdu PRIVATE srsdu_base srsran_flexible_o_du_split_dynamic)
notify_binary_target(srsdu)


# Add the targets for the rest of the splits.
set(EXTENSIONS du_split_6 du_split_7_2 du_split_8)
set(LIBRARIES srsran_flexible_o_du_split_6 srsran_flexible_o_du_split_7_2 srsran_flexible_o_du_split_8)

foreach (NAME LIB IN ZIP_LISTS EXTENSIONS LIBRARIES)
    # Prepend srs to the target name to make it consistent with the dynamic one.
    set(TARGET "srs${NAME}")
    add_executable(${TARGET} du.cpp)
    add_backward(${TARGET})
    target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${TARGET} PRIVATE srsdu_base ${LIB})
    set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../${NAME}"  # Path to the binary.
            RUNTIME_OUTPUT_NAME "srsdu"                                        # Binary name.
            EXCLUDE_FROM_ALL 1)                                                # Do not build by default.
    notify_binary_target(${TARGET})
endforeach ()

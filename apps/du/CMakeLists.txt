#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_appconfig_cli11_schema.cpp
        du_appconfig_validators.cpp
        du_appconfig_translators.cpp
        du_appconfig_yaml_writer.cpp)


add_library(srsdu_base STATIC ${SOURCES})
target_link_libraries(srsdu_base PRIVATE
        srsran_app_services
        srsran_f1u_du_split_connector
        srsran_cu_cp
	srsran_f1u_cu_up_split_connector # TODO FIXME!!!
        srsran_network
        srsran_e2
        e2ap_asn1
        srsran_pcap
        srsran_support
        srsran_versioning
        srsran_f1c_gateway
        srsran_cu_up) # TODO: Delete

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
set(EXTENSIONS du_split_7_2 du_split_8)
set(LIBRARIES srsran_flexible_o_du_split_7_2 srsran_flexible_o_du_split_8)

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

#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_subdirectory(helpers)

# Build the flexible du split 8 library when it is selected in the definition.
if (DU_SPLIT_8)
    set(SOURCES
            split_8_du_application_unit_impl.cpp
            split_8_du_factory.cpp
            split_8_du_unit_cli11_schema.cpp
            split_8_du_unit_config_validator.cpp
            split_8_du_unit_config_yaml_writer.cpp)

    add_library(srsran_flexible_du STATIC ${SOURCES})
    target_include_directories(srsran_flexible_du PRIVATE ${CMAKE_SOURCE_DIR})

    # Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
    if (DPDK_FOUND)
       set_source_files_properties(split_8_du_factory.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
    endif (DPDK_FOUND)

    target_link_libraries(srsran_flexible_du
            srsran_du_wrapper
            srsran_pcap
            srsran_app_services
            srsran_fapi_app_unit
            srsran_du_low_unit_helpers
            srsran_split_8_app_unit_helpers
            srsran_du_high_unit_helpers
            srsran_flexible_du_helpers)

endif ()

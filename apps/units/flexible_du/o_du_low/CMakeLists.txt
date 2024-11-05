#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
            du_low_config_cli11_schema.cpp
            du_low_config_translator.cpp
            du_low_config_validator.cpp
            du_low_config_yaml_writer.cpp
            o_du_low_unit_factory.cpp)

add_library(srsran_o_du_low_unit_helpers STATIC ${SOURCES})
target_include_directories(srsran_o_du_low_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
set(DU_LOW_UNIT_HELPERS_LIBRARIES srsran_upper_phy srsran_cpu_affinities_helper)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(${SOURCES} PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_UNIT_HELPERS_LIBRARIES srsran_hal_pusch
        srsran_hal_pdsch
        srsran_hal_bbdev)
endif (DPDK_FOUND)

target_link_libraries(srsran_o_du_low_unit_helpers PUBLIC ${DU_LOW_UNIT_HELPERS_LIBRARIES})

#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(du_high)
add_subdirectory(du_low)

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})

add_library(srsran_o_du STATIC
        o_du_impl.cpp
        o_du_factory.cpp)

target_link_libraries(srsran_o_du PUBLIC srsran_o_du_high srsran_o_du_low)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(o_du_factory.cpp o_du_impl.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES srsran_hal_pusch
           srsran_hal_pdsch
           srsran_hal_bbdev)
endif (DPDK_FOUND)

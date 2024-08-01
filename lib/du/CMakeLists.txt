#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})

add_library(srsran_du_high_wrapper STATIC
        du_high_wrapper_factory.cpp
        du_high_wrapper_impl.cpp
        adapters/fapi_factory.cpp)
target_link_libraries(srsran_du_high_wrapper PUBLIC srsran_du_config_validators srsran_du_high srsran_mac_fapi_adaptor srsran_fapi)


add_library(srsran_du_wrapper STATIC
        du_wrapper_impl.cpp
        du_wrapper_factory.cpp)
set(DU_WRAPPER_LIBRARIES srsran_du_high_wrapper srsran_du_low_wrapper)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(du_wrapper_factory.cpp du_wrapper_impl.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES hal_hwacc_pusch
        hal_hwacc_pdsch
        hal_bbdev_factory)
endif (DPDK_FOUND)
target_link_libraries(srsran_du_wrapper PUBLIC ${DU_WRAPPER_LIBRARIES})

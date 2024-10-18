#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_low_factory.cpp
        du_low_impl.cpp
        du_low_wrapper_factory.cpp
        du_low_wrapper_impl.cpp)

add_library(srsran_du_low_wrapper STATIC ${SOURCES})
set(DU_LOW_LIBRARIES srsran_phy_fapi_adaptor srsran_upper_phy srsran_channel_precoder)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(${SOURCES} PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES srsran_hal_pusch
        srsran_hal_pdsch
        srsran_hal_bbdev)
endif (DPDK_FOUND)
target_link_libraries(srsran_du_low_wrapper PUBLIC ${DU_LOW_LIBRARIES})

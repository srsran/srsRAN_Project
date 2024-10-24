#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        flexible_du_factory.cpp
        flexible_du_impl.cpp
        multicell_flexible_du_factory.cpp
        multicell_flexible_du_impl.cpp
        o_du_high_factory.cpp)

add_library(srsran_flexible_du_helpers STATIC ${SOURCES})
target_include_directories(srsran_flexible_du_helpers PRIVATE ${CMAKE_SOURCE_DIR})

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(flexible_du_factory.cpp multicell_flexible_du_factory.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
endif (DPDK_FOUND)

target_link_libraries(srsran_flexible_du_helpers
        srsran_o_du
        srsran_ru_dummy
        srsran_pcap
        srsran_app_services
        srsran_fapi_app_unit
        srsran_o_du_low_unit_helpers
        srsran_o_du_high_unit_helpers)

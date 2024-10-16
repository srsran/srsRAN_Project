#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(pusch)

set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PDSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PDSCH_HWACC)
endif ()


add_library(hal_hwacc_pdsch STATIC hw_accelerator_factories.cpp)

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(hal_hwacc_pdsch hal_bbdev_factory hal_dpdk ${DPDK_LIBRARIES} dl)

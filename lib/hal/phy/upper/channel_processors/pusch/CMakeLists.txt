#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PUSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PUSCH_HWACC)
endif ()


add_library(srsran_hal_pusch STATIC hw_accelerator_factories.cpp
                                   ext_harq_buffer_context_repository_factory.cpp)

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(srsran_hal_pusch srsran_hal_bbdev hal_dpdk ${DPDK_LIBRARIES} dl)

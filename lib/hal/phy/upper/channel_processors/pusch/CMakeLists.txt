#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(HAL_FACTORIES_DEFINITIONS "")

if (ENABLE_PUSCH_HWACC)
    list(APPEND HAL_FACTORIES_DEFINITIONS ENABLE_PUSCH_HWACC)
endif ()


add_library(hal_hwacc_pusch STATIC hal_factories.cpp
                                   hw_accelerator_pusch_dec_impl.cpp
                                   hw_accelerator_pusch_dec_acc100_impl.cpp
                                   ext_harq_buffer_context_repository_factory.cpp)

set_source_files_properties(hal_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HAL_FACTORIES_DEFINITIONS}")

target_link_libraries(hal_hwacc_pusch hal_bbdev hal_dpdk ${DPDK_LIBRARIES})

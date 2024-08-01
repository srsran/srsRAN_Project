#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(bbdev)

set(SOURCES
  dpdk_eal_factory.cpp
  dpdk.cpp
  mbuf_pool_factory.cpp
)

add_library(hal_dpdk STATIC ${SOURCES})
target_compile_options(hal_dpdk PRIVATE ${DPDK_CFLAGS})
target_link_libraries(hal_dpdk srslog ${DPDK_LIBRARIES})

add_to_exported_libs(hal_dpdk)

#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES 
  dpdk_eal.cpp
  dpdk_eal_factory.cpp
  dpdk.cpp
  mbuf_pool.cpp
  mbuf_pool_factory.cpp
)

add_library(hal_dpdk STATIC ${SOURCES})

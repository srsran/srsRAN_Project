/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dpdk.h"

using namespace srsran;
using namespace dpdk;

/// \file
/// \brief EAL functions.

bool dpdk::eal_init(int argc, char** argv, srslog::basic_logger& logger)
{
  if (::rte_eal_init(argc, argv) < 0) {
    logger.error("dpdk: rte_eal_init failed");
    return false;
  }

  return true;
}

/// \file
/// \brief Common memory pool functions.

::rte_mempool* dpdk::create_mem_pool(const char*           pool_name,
                                     int                   socket,
                                     unsigned              n_mbuf,
                                     unsigned              mempool_cache_size,
                                     unsigned              mbuf_data_size,
                                     srslog::basic_logger& logger)
{
  // Create a new mbuf pool for the hardware-accelerated functions.
  ::rte_mempool* mbuf_pool =
      ::rte_pktmbuf_pool_create(pool_name, n_mbuf, mempool_cache_size, 0, mbuf_data_size, socket);

  if (mbuf_pool == nullptr) {
    logger.error("dpdk: create_mbuf_pool '{}' failed (size {})", pool_name, mbuf_data_size);
  }

  return mbuf_pool;
}

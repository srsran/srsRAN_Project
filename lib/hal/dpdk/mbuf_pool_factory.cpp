/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/mbuf_pool_factory.h"
#include "dpdk.h"

using namespace srsran;
using namespace dpdk;

std::unique_ptr<mbuf_pool> srsran::dpdk::create_mbuf_pool(const char*           pool_name,
                                                          int                   socket,
                                                          const mempool_config& cfg,
                                                          srslog::basic_logger& logger)
{
  // Create a new memory pool.
  ::rte_mempool* pool =
      create_mem_pool(pool_name, socket, cfg.n_mbuf, cfg.mempool_cache_size, cfg.mbuf_data_size, logger);
  if (pool == nullptr) {
    return nullptr;
  }

  return std::make_unique<mbuf_pool>(pool);
}

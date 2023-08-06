/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

rte_mempool* dpdk::create_mem_pool(const char*           pool_name,
                                   int                   socket,
                                   unsigned              n_mbuf,
                                   unsigned              mempool_cache_size,
                                   unsigned              mbuf_data_size,
                                   srslog::basic_logger& logger)
{
  // Create a new mbuf pool for the hardware-accelerated functions.
  rte_mempool* mbuf_pool = ::rte_pktmbuf_pool_create(pool_name, n_mbuf, mempool_cache_size, 0, mbuf_data_size, socket);

  if (mbuf_pool == nullptr) {
    logger.error("dpdk: create_mbuf_pool '{}' failed (size {})", pool_name, mbuf_data_size);
  }

  return mbuf_pool;
}

void dpdk::free_mem_pool(rte_mempool& mem_pool)
{
  ::rte_mempool_free(&mem_pool);
}

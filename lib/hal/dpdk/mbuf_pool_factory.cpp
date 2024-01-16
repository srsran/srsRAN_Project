/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

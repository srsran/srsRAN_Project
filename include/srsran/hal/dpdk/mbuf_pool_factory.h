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

#pragma once

#include "srsran/hal/dpdk/mbuf_pool.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {
namespace dpdk {

/// Configuration structure of a dpdk memory buffer pool.
struct mempool_config {
  /// Number of elements in the mbuf pool.
  unsigned n_mbuf = 2048;
  /// Size of the per-core object cache.
  unsigned mempool_cache_size = 128;
  /// Size of data buffer in each mbuf.
  unsigned mbuf_data_size = 2048;
};

/// Returns a mbuf_pool instance on success, otherwise returns nullptr.
std::unique_ptr<mbuf_pool>
create_mbuf_pool(const char* pool_name, int socket, const mempool_config& cfg, srslog::basic_logger& logger);

} // namespace dpdk
} // namespace srsran

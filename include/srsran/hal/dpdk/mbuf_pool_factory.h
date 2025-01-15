/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

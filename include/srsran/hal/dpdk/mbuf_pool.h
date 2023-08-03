/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rte_mempool.h"

namespace srsran {
namespace dpdk {

/// RAII wrapping for dpdk memory buffer pool structures.
class mbuf_pool
{
public:
  /// Constructor.
  /// \param[in] pool_ Pointer to a dpdk memory pool.
  explicit mbuf_pool(rte_mempool* pool_);

  /// Default destructor.
  ~mbuf_pool();

  // Returns a pointer to the actual memory pool object.
  /// \return Pointer to the memory pool.
  rte_mempool* get_pool() { return &pool; }

  // Returns a pointer to a constant memory pool object.
  /// \return Pointer to a constant memory pool.
  const rte_mempool* get_pool() const { return &pool; }

private:
  rte_mempool& pool;
};

} // namespace dpdk
} // namespace srsran

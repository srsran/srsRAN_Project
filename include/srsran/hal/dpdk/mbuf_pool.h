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

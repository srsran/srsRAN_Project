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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/support/event_tracing.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// Implements a generic resource grid pool that zeroes the resource grids after a number of slots.
class resource_grid_pool_asynchronous_impl : public resource_grid_pool
{
public:
  using resource_grid_ptr = std::unique_ptr<resource_grid>;

  /// \brief Constructs a resource grid pool.
  /// \param expire_timeout_slots_ Resource grid timeout expiration in slots.
  /// \param async_executor_       Asynchronous housekeeping executor.
  /// \param grids_                Resource grids.
  resource_grid_pool_asynchronous_impl(unsigned                                    expire_timeout_slots_,
                                       task_executor&                              async_executor_,
                                       std::vector<std::unique_ptr<resource_grid>> grids);

  // See interface for documentation.
  resource_grid& get_resource_grid(const resource_grid_context& context) override;

private:
  struct reserved_resource_grid {
    slot_point expiration;
    unsigned   grid_id;
  };

  /// PHY logger.
  srslog::basic_logger& logger;
  /// Grid expiration timeout in slots.
  unsigned expire_timeout_slots;
  /// Actual pool of resource grids.
  std::vector<resource_grid_ptr> grids;
  /// Pool of resource grid zero set string for tracing.
  std::vector<std::string> grids_str_zero;
  /// Pool of resource grid reservation string for tracing.
  std::vector<std::string> grids_str_reserved;
  /// Reserved resource grids.
  ring_buffer<reserved_resource_grid, false> reserved;
  /// Queue of resource grids ready to use.
  concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep> available;
  /// Asynchronous task executor.
  task_executor& async_executor;
};

} // namespace srsran

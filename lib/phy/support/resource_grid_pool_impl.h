/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/tracing/event_tracing.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// \brief Implements a resource grid pool.
///
/// It zeroes the resource grids asynchronously upon their scope release if it is given an asynchronous executor.
/// Otherwise, it does not zero the resource grid.
class resource_grid_pool_impl : public resource_grid_pool, private shared_resource_grid::pool_interface
{
public:
  using resource_grid_ptr = std::unique_ptr<resource_grid>;

  /// \brief Constructs a resource grid pool.
  /// \param async_executor_ Asynchronous housekeeping executor.
  /// \param grids_          Resource grids.
  resource_grid_pool_impl(task_executor* async_executor_, std::vector<resource_grid_ptr> grids_);

  /// \brief The destructor checks that all resource grids have been returned to the pool.
  ///
  /// The resource grids that are still active in at least one scope get dereferenced if the pool is destructed prior
  /// their context release. This triggers a segmentation fault signal.
  ///
  /// A fatal error is triggered if not all the resource grids have been returned to the pool.
  ~resource_grid_pool_impl() override;

  // See resource_grid_pool interface for documentation.
  shared_resource_grid allocate_resource_grid(slot_point slot) override;

private:
  /// Reference counter value to indicate the availability of a resource grid.
  static constexpr unsigned ref_counter_available = std::numeric_limits<unsigned>::max();

  // See shared_resource_grid::pool_interface for documentation.
  resource_grid& get(unsigned identifier) override;

  // See shared_resource_grid::pool_interface for documentation.
  void notify_release_scope(unsigned identifier) override;

  /// PHY logger.
  srslog::basic_logger& logger;
  /// Actual pool of resource grids.
  std::vector<resource_grid_ptr> grids;
  /// Counts the resource grid requests.
  unsigned counter = 0;
  /// \brief Resource grid scope count.
  ///
  /// A resource grid is available when the counter is equal to \c ref_counter_available.
  span<std::atomic<unsigned>> grids_scope_count;
  /// Pool of resource grid zero set string for tracing.
  std::vector<std::string> grids_str_zero;
  /// Pool of resource grid reservation string for tracing.
  std::vector<std::string> grids_str_reserved;
  /// Asynchronous task executor.
  task_executor* async_executor;
};

} // namespace srsran

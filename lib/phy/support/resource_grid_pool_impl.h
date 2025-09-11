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
#include "srsran/support/synchronization/stop_event.h"
#include "srsran/support/tracing/event_tracing.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// \brief Resource grid state control.
///
/// This class controls the number of scopes in which a shared resource grid is present. Also, it guarantees that the
/// resource grid is returned to the pool prior the destruction.
class resource_grid_pool_wrapper : private shared_resource_grid::pool_interface
{
public:
  /// Constructs a resource grid wrapper for the pool.
  resource_grid_pool_wrapper(std::unique_ptr<resource_grid> grid_, task_executor* async_executor_) :
    grid(std::move(grid_)), async_executor(async_executor_)
  {
    srsran_assert(grid, "Invalid resource grid pointer.");
  }

  /// Forbid copy constructor.
  resource_grid_pool_wrapper(const resource_grid_pool_wrapper& other) = delete;

  /// Move constructor.
  resource_grid_pool_wrapper(resource_grid_pool_wrapper&& other) :
    grid(std::move(other.grid)), async_executor(other.async_executor)
  {
  }

  /// \brief Try to reserve the resource grid.
  ///
  /// \return True if the resource grid is reserved successfully. Otherwise, false.
  shared_resource_grid try_reserve(stop_event_token token);

private:
  /// Reference counter value to indicate the availability of a resource grid.
  static constexpr unsigned ref_counter_available = std::numeric_limits<unsigned>::max();

  /// \brief Release resource grid.
  ///
  /// This method transitions the reference count from zero to available.
  ///
  /// \remark An assertion is triggered if the resource grid is present in any scope.
  void release();

  // See the shared_resource_grid::pool_interface interface for documentation.
  void notify_release_scope() override;

  // See the shared_resource_grid::pool_interface interface for documentation.
  resource_grid& get() override;

  /// Internal resource grid;
  std::unique_ptr<resource_grid> grid;
  /// Asynchronous task executor.
  task_executor* async_executor;
  /// \brief Internal resource grid scope count.
  ///
  /// A resource grid is available when the counter is equal to \c ref_counter_available.
  std::atomic<unsigned> scope_count = ref_counter_available;
  /// Signaling mechanism for safe stop.
  stop_event_token stop_token;
};

/// \brief Implements a resource grid pool.
///
/// It zeroes the resource grids asynchronously upon their scope release if it is given an asynchronous executor.
/// Otherwise, it does not zero the resource grid.
class resource_grid_pool_impl : public resource_grid_pool
{
public:
  /// \brief Constructs a resource grid pool.
  /// \param grids_ Resource grids.
  resource_grid_pool_impl(std::vector<resource_grid_pool_wrapper> grids_) :
    logger(srslog::fetch_basic_logger("PHY", true)), grids(std::move(grids_))
  {
  }

  /// The destructor waits until all resource grids are available to avoid dereferencing the reference counter.
  ~resource_grid_pool_impl() override;

  // See resource_grid_pool interface for documentation.
  shared_resource_grid allocate_resource_grid(slot_point slot) override;

private:
  /// PHY logger.
  srslog::basic_logger& logger;
  /// Counts the resource grid requests.
  unsigned counter = 0;
  /// Resource grid state controllers. There is a controller for each grid.
  std::vector<resource_grid_pool_wrapper> grids;
  /// Stop control.
  stop_event_source stop_control;
};

} // namespace srsran

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

#include "resource_grid_pool_asynchronous_impl.h"
#include "srsran/adt/optional.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_pool_asynchronous_impl::resource_grid_pool_asynchronous_impl(unsigned       expire_timeout_slots_,
                                                                           task_executor& async_executor_,
                                                                           std::vector<resource_grid_ptr> grids_) :
  logger(srslog::fetch_basic_logger("PHY", true)),
  expire_timeout_slots(expire_timeout_slots_),
  grids(std::move(grids_)),
  grids_str_zero(grids.size()),
  grids_str_reserved(grids.size()),
  reserved(grids.size()),
  available(grids.size()),
  async_executor(async_executor_)
{
  // Make sure the expiration timeout is consistent with the number of grids. If the expiration time is greater than
  // or equal to the number of grids, the pool blocks.
  srsran_assert(expire_timeout_slots < grids.size(),
                "The grid expiration time (i.e., {}) must be lesser than the number of grids (i.e., {}).",
                expire_timeout_slots,
                grids.size());

  // Move grids to available list.
  for (unsigned i_grid = 0, i_grid_end = grids.size(); i_grid != i_grid_end; ++i_grid) {
    grids_str_zero[i_grid]     = "set_all_zero#" + std::to_string(i_grid);
    grids_str_reserved[i_grid] = "rg_reserved#" + std::to_string(i_grid);

    bool success = available.push_blocking(i_grid);
    srsran_assert(success, "Failed to push grid.");
  }
}

resource_grid& resource_grid_pool_asynchronous_impl::get_resource_grid(const resource_grid_context& context)
{
  // While the reserved buffer is not empty and the first element is expired.
  while (!reserved.empty() && (reserved.top().expiration <= context.slot)) {
    // Read first element.
    reserved_resource_grid grid = reserved.top();

    // Extract grid identifier.
    unsigned grid_id = grid.grid_id;

    // Skip setting to zero if the grid is empty.
    if (grids[grid_id]->get_reader().is_empty()) {
      reserved.pop();
      available.push_blocking(grid_id);
      continue;
    }

    // Create lambda function for setting the grid to zero.
    auto set_all_zero_func = [this, grid_id]() {
      trace_point tp = l1_tracer.now();

      // Set grid to zero.
      grids[grid_id]->set_all_zero();

      // Queue available grid.
      bool success = available.push_blocking(grid_id);
      srsran_assert(success, "Failed to push grid.");

      l1_tracer << trace_event(grids_str_zero[grid_id].c_str(), tp);
    };

    // Try to execute the asynchronous housekeeping task.
    bool success = async_executor.execute(set_all_zero_func);

    // Pop the resource grid identifier if it was successfully executed. Otherwise, it skips zeroing.
    if (success) {
      reserved.pop();
    } else {
      logger.warning(context.slot.sfn(), context.slot.slot_index(), "Failed to enqueue grid zeroing task.");
      break;
    }
  }

  // Trace point for grid reservation.
  trace_point tp = l1_tracer.now();

  // Pop first available grid.
  optional<unsigned> grid = available.pop_blocking();
  srsran_assert(grid.has_value(), "Failed to pop grid.");

  // Prepare reserved grid and enqueue.
  reserved_resource_grid reserved_grid;
  reserved_grid.expiration = context.slot + expire_timeout_slots;
  reserved_grid.grid_id    = grid.value();
  reserved.push(reserved_grid);

  // Trace the resource grid reservation.
  l1_tracer << trace_event(grids_str_reserved[grid.value()].c_str(), tp);

  return *grids[reserved_grid.grid_id];
}

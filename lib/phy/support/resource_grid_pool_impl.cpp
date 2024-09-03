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

#include "resource_grid_pool_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_pool_impl::resource_grid_pool_impl(task_executor*                 async_executor_,
                                                 std::vector<resource_grid_ptr> grids_) :
  logger(srslog::fetch_basic_logger("PHY", true)),
  grids(std::move(grids_)),
  grids_scope_count(grids.size()),
  grids_str_zero(grids.size()),
  grids_str_reserved(grids.size()),
  async_executor(async_executor_)
{
  // Create tracing list.
  for (unsigned i_grid = 0, i_grid_end = grids.size(); i_grid != i_grid_end; ++i_grid) {
    grids_str_zero[i_grid]     = "set_all_zero#" + std::to_string(i_grid);
    grids_str_reserved[i_grid] = "rg_reserved#" + std::to_string(i_grid);
  }

  // All grids scopes must be zero.
  std::fill(grids_scope_count.begin(), grids_scope_count.end(), ref_counter_available);
}

resource_grid_pool_impl::~resource_grid_pool_impl()
{
  // Ensure that all the grids returned to the pool. The application will result in segmentation fault.
  report_fatal_error_if_not(std::all_of(grids_scope_count.cbegin(),
                                        grids_scope_count.cend(),
                                        [](auto& e) { return e == ref_counter_available; }),
                            "Not all the resource grids have returned to the pool.");
}

shared_resource_grid resource_grid_pool_impl::allocate_resource_grid(const resource_grid_context& context)
{
  // Trace point for grid reservation.
  trace_point tp = l1_tracer.now();

  // Select an identifier from the current request counter.
  unsigned identifier = counter;

  // Increment request counter and wrap it with the number of grids.
  counter = (counter + 1) % grids.size();

  // Select counter for the selected identifier.
  std::atomic<unsigned>& ref_count = grids_scope_count[identifier];

  // Try to reset the reference counter.
  unsigned expected  = ref_counter_available;
  bool     available = ref_count.compare_exchange_strong(expected, 1, std::memory_order_acq_rel);

  // Return an invalid grid if not available.
  if (!available) {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Resource grid with identifier {} is not available.",
                   identifier);
    return {};
  }

  // Trace the resource grid reservation.
  l1_tracer << trace_event(grids_str_reserved[identifier].c_str(), tp);

  return {*this, ref_count, identifier};
}

resource_grid& resource_grid_pool_impl::get(unsigned identifier)
{
  srsran_assert(identifier < grids.size(),
                "RG identifier (i.e., {}) is out of range {}.",
                identifier,
                interval<unsigned>(0, grids.size()));
  return *grids[identifier];
}

void resource_grid_pool_impl::notify_release_scope(unsigned identifier)
{
  // verify the identifier is valid.
  srsran_assert(identifier < grids.size(),
                "RG identifier (i.e., {}) is out of range {}.",
                identifier,
                interval<unsigned>(0, grids.size()));

  // Skip zeroing if the grid is empty.
  if (grids[identifier]->get_reader().is_empty()) {
    grids_scope_count[identifier] = ref_counter_available;
    return;
  }

  // If the pool is not configured with an asynchronous executor, it skips the zeroing process.
  if (async_executor == nullptr) {
    grids_scope_count[identifier] = ref_counter_available;
    return;
  }

  // Create lambda function for setting the grid to zero.
  auto set_all_zero_func = [this, identifier]() {
    trace_point tp = l1_tracer.now();

    // Set grid to zero.
    grids[identifier]->set_all_zero();

    // Make the grid available.
    grids_scope_count[identifier] = ref_counter_available;

    l1_tracer << trace_event(grids_str_zero[identifier].c_str(), tp);
  };

  // Try to execute the asynchronous housekeeping task.
  bool success = async_executor->execute(set_all_zero_func);

  // Ensure the resource grid is marked as available even if it is not empty.
  // Avoid warnings about failure to prevent false alarms during gNb teardown.
  if (!success) {
    grids_scope_count[identifier] = ref_counter_available;
  }
}

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

#include "resource_grid_pool_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/srsran_assert.h"
#include <mutex>
#include <thread>

using namespace srsran;

shared_resource_grid resource_grid_pool_wrapper::try_reserve(srsran::stop_event_token token)
{
  // Try to transition from available to one scope.
  unsigned expected = ref_counter_available;
  bool     success  = scope_count.compare_exchange_strong(expected, 1, std::memory_order_acq_rel);

  // Return an invalid shared grid if the state transition is unsuccessful.
  if (!success) {
    return {};
  }

  // Keep token if successful.
  stop_token = std::move(token);

  // Create valid shared resource grid.
  return shared_resource_grid{*this, scope_count};
}

void resource_grid_pool_wrapper::release()
{
  // Move token to the scope of this method before transitioning to available.
  stop_event_token token = std::move(stop_token);

  // Transition to available.
  [[maybe_unused]] unsigned prev_scope_count = scope_count.exchange(ref_counter_available, std::memory_order_acq_rel);

  // Assert that the grid is not present in any scope before becoming available.
  srsran_assert((prev_scope_count == ref_counter_available) || (prev_scope_count == 0),
                "The resource grid state cannot transition to available while it is present in {} scopes.",
                prev_scope_count);
}

void resource_grid_pool_wrapper::notify_release_scope()
{
  // Skip zeroing if the grid is empty.
  if (grid->get_reader().is_empty()) {
    release();
    return;
  }

  // If the pool is not configured with an asynchronous executor, it skips the zeroing process.
  if (async_executor == nullptr) {
    release();
    return;
  }

  // Create lambda function for setting the grid to zero.
  auto set_all_zero_func = [this]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    // Set grid to zero.
    grid->set_all_zero();

    // Make the grid available.
    release();
  };

  // Try to execute the asynchronous housekeeping task.
  bool success = async_executor->defer(set_all_zero_func);

  // Ensure the resource grid is marked as available even if it is not empty.
  // Avoid warnings about failure to prevent false alarms during gNb teardown.
  if (!success) {
    release();
  }
}

resource_grid& resource_grid_pool_wrapper::get()
{
  return *grid;
}

resource_grid_pool_impl::~resource_grid_pool_impl()
{
  stop_control.stop();
}

shared_resource_grid resource_grid_pool_impl::allocate_resource_grid(slot_point slot)
{
  // Get a stop token, return an invalid grid if it is stopping.
  stop_event_token token = stop_control.get_token();
  if (token.stop_requested()) {
    return {};
  }

  // Select an identifier from the current request counter.
  unsigned identifier = counter;

  // Increment request counter and wrap it with the number of grids.
  counter = (counter + 1) % grids.size();

  // Try reserving the resource grid.
  shared_resource_grid grid = grids[identifier].try_reserve(std::move(token));

  // Log a warning message if the grid is not available.
  if (!grid) {
    logger.warning(slot.sfn(), slot.slot_index(), "Resource grid with identifier {} is not available.", identifier);
  }

  return grid;
}

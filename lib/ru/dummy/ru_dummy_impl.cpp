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

#include "ru_dummy_impl.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ru/ru_dummy_configuration.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <chrono>
#include <cstdint>
#include <thread>

using namespace srsran;

ru_dummy_impl::ru_dummy_impl(const srsran::ru_dummy_configuration& config, ru_dummy_dependencies dependencies) noexcept
  :
  logger(*dependencies.logger),
  executor(*dependencies.executor),
  timing_notifier(*dependencies.timing_notifier),
  slot_duration(1000 / pow2(to_numerology_value(config.scs))),
  max_processing_delay_slots(config.max_processing_delay_slots),
  current_slot(config.scs, config.max_processing_delay_slots)
{
  srsran_assert(config.max_processing_delay_slots > 0, "The maximum processing delay must be greater than 0.");
  srsran_assert(dependencies.logger != nullptr, "Invalid logger.");
  srsran_assert(dependencies.executor != nullptr, "Invalid executor.");
  srsran_assert(dependencies.symbol_notifier != nullptr, "Invalid symbol notifier.");
  srsran_assert(dependencies.timing_notifier != nullptr, "Invalid timing notifier.");

  sectors.reserve(config.nof_sectors);
  for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
    sectors.emplace_back(i_sector,
                         config.rx_rg_nof_prb,
                         config.rx_rg_nof_ports,
                         config.rx_prach_nof_ports,
                         config.dl_processing_delay,
                         logger,
                         *dependencies.symbol_notifier);
  }
}

void ru_dummy_impl::start()
{
  state previous_state = current_state.exchange(state::running);
  srsran_assert(previous_state == state::idle, "Invalid state.");
  report_fatal_error_if_not(executor.execute([this]() { loop(); }), "Failed to execute loop method.");
}

void ru_dummy_impl::stop()
{
  // Signal stop to asynchronous thread.
  state previous_state = current_state.exchange(state::wait_stop);
  srsran_assert(previous_state == state::running, "Invalid state.");

  // Wait for the state to transition to stop.
  while (current_state != state::stopped) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void ru_dummy_impl::loop()
{
  // Check stop condition.
  if (current_state == state::wait_stop) {
    current_state = state::stopped;
    return;
  }

  // Get the time since the epoch.
  auto time_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

  // Calculate the number of slots passed since the epoch and wrap with the number of slots per system frame.
  uint64_t slot_count = (time_since_epoch / slot_duration) % current_slot.nof_slots_per_system_frame();

  // Check if the slot changed and notify the next slot.
  if (slot_count != current_slot.system_slot()) {
    // Prepare the current slot.
    current_slot = slot_point(current_slot.scs(), slot_count);

    // Notify new slot boundary.
    timing_notifier.on_tti_boundary(current_slot + max_processing_delay_slots);

    // Notify UL half slot.
    timing_notifier.on_ul_half_slot_boundary(current_slot);

    // Notify UL full slot.
    timing_notifier.on_ul_full_slot_boundary(current_slot);

    // Notify the slot boundary in all the sectors.
    for (auto& sector : sectors) {
      sector.new_slot_boundary(current_slot);
    }
  } else {
    // Make sure a minimum time between loop executions without crossing boundaries.
    std::this_thread::sleep_for(minimum_loop_time);
  }

  // Feed back the execution of this task.
  report_fatal_error_if_not(executor.defer([this]() { loop(); }), "Failed to execute loop method.");
}

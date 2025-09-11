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

#include "ru_dummy_impl.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ru/dummy/ru_dummy_configuration.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <chrono>
#include <cstdint>
#include <thread>

using namespace srsran;

static inline uint64_t get_current_system_slot(std::chrono::microseconds slot_duration,
                                               uint64_t                  nof_slots_per_hyper_system_frame)
{
  // Get the time since the epoch.
  auto time_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

  return (time_since_epoch / slot_duration) % nof_slots_per_hyper_system_frame;
}

ru_dummy_impl::ru_dummy_impl(const ru_dummy_configuration& config, ru_dummy_dependencies dependencies) noexcept :
  are_metrics_enabled(config.are_metrics_enabled),
  logger(dependencies.logger),
  executor(*dependencies.executor),
  timing_notifier(dependencies.timing_notifier),
  slot_duration(static_cast<unsigned>(config.time_scaling * 1000.0 / pow2(to_numerology_value(config.scs)))),
  max_processing_delay_slots(config.max_processing_delay_slots),
  current_slot(config.scs, config.max_processing_delay_slots),
  metrics_collector({})
{
  srsran_assert(config.max_processing_delay_slots > 0, "The maximum processing delay must be greater than 0.");

  sectors.reserve(config.nof_sectors);
  for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
    sectors.emplace_back(i_sector,
                         config.rx_rg_nof_prb,
                         config.rx_rg_nof_ports,
                         config.rx_prach_nof_ports,
                         config.dl_processing_delay,
                         logger,
                         dependencies.symbol_notifier,
                         dependencies.error_notifier);
  }

  metrics_collector = ru_dummy_metrics_collector(sectors);
}

void ru_dummy_impl::start()
{
  // Get initial system slot.
  uint64_t initial_system_slot =
      get_current_system_slot(slot_duration, current_slot.nof_slots_per_hyper_system_frame());
  current_slot = slot_point(current_slot.numerology(), initial_system_slot);

  // Start the loop execution.
  defer_loop();
}

void ru_dummy_impl::stop()
{
  // Stop each of the sectors.
  for (auto& sector : sectors) {
    sector.stop();
  }

  // Signal stop to asynchronous thread.
  stop_control.stop();
}

void ru_dummy_impl::defer_loop()
{
  // Do not defer if stop was requested.
  auto token = stop_control.get_token();
  if (stop_control.stop_was_requested()) {
    return;
  }

  // Create loop task.
  unique_function<void(), default_unique_task_buffer_size, true> task =
      [this, defer_token = std::move(token)]() noexcept SRSRAN_RTSAN_NONBLOCKING { loop(); };

  // Actual defer.
  report_fatal_error_if_not(executor.defer(std::move(task)), "Failed to execute loop method.");
}

void ru_dummy_impl::loop()
{
  // Get the current system slot from the system time.
  uint64_t slot_count = get_current_system_slot(slot_duration, current_slot.nof_slots_per_hyper_system_frame());

  // Make sure a minimum time between loop executions without crossing boundaries.
  if (slot_count == current_slot.system_slot()) {
    SRSRAN_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(minimum_loop_time);
  }

  // Advance the current slot until it is equal to the slot given by the system time.
  while (slot_count != current_slot.system_slot()) {
    // Increment current slot.
    ++current_slot;

    // Notify new slot boundary.
    tti_boundary_context context;
    context.slot       = current_slot + max_processing_delay_slots;
    context.time_point = std::chrono::system_clock::now() + (slot_duration * max_processing_delay_slots);
    timing_notifier.on_tti_boundary(context);

    // Notify UL half slot.
    timing_notifier.on_ul_half_slot_boundary(current_slot);

    // Notify UL full slot.
    timing_notifier.on_ul_full_slot_boundary(current_slot);

    // Notify the slot boundary in all the sectors.
    for (auto& sector : sectors) {
      sector.new_slot_boundary(current_slot);
    }
  }

  // Feed back the execution of this task.
  defer_loop();
}

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

#include "ru_dummy_sector.h"
#include "srsran/adt/interval.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ru/ru.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_dummy_configuration.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>

namespace srsran {

/// \brief Implements a dummy radio unit.
///
/// The dummy radio unit is a radio unit designed for performance and stability testing purposes. It does not connect to
/// any RF device.
///
/// It determines the timing from the system time.
class ru_dummy_impl : public radio_unit,
                      private ru_controller,
                      private ru_downlink_plane_handler,
                      private ru_uplink_plane_handler
{
public:
  /// Creates a dummy radio unit from its configuration.
  explicit ru_dummy_impl(const ru_dummy_configuration& config, ru_dummy_dependencies dependencies) noexcept;

  // See radio_unit interface for documentation.
  ru_controller& get_controller() override { return *this; }

  // See radio_unit interface for documentation.
  ru_downlink_plane_handler& get_downlink_plane_handler() override { return *this; }

  // See radio_unit interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override { return *this; }

private:
  /// Possible internal states.
  enum class state : uint8_t { idle = 0, running, wait_stop, stopped };
  /// Minimum loop time.
  const std::chrono::microseconds minimum_loop_time = std::chrono::microseconds(10);

  // See ru_controller for documentation.
  void start() override;

  // See ru_controller for documentation.
  void stop() override;

  // See interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override { return false; }

  // See interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override { return false; }

  // See ru_downlink_plane_handler for documentation.
  void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    srsran_assert(sector_range.contains(context.sector),
                  "Sector identifier (i.e., {}) is out-of-bounds {}.",
                  context.sector,
                  sector_range);
    sectors[context.sector].handle_dl_data(context, grid);
  }

  // See ru_uplink_plane_handler for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    srsran_assert(sector_range.contains(context.sector),
                  "Sector identifier (i.e., {}) is out-of-bounds {}.",
                  context.sector,
                  sector_range);
    sectors[context.sector].handle_prach_occasion(context, buffer);
  }

  // See ru_uplink_plane_handler for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    srsran_assert(sector_range.contains(context.sector),
                  "Sector identifier (i.e., {}) is out-of-bounds {}.",
                  context.sector,
                  sector_range);
    sectors[context.sector].handle_new_uplink_slot(context, grid);
  }

  /// Loop execution task.
  void loop();

  /// Ru logger.
  srslog::basic_logger& logger;
  /// Internal executor.
  task_executor& executor;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Internal state.
  std::atomic<state> current_state = {state::idle};
  /// Slot time in microseconds.
  std::chrono::microseconds slot_duration;
  /// Number of slots is notified in advance of the transmission time.
  unsigned max_processing_delay_slots;
  /// Current slot.
  slot_point current_slot;
  /// Radio unit sectors.
  std::vector<ru_dummy_sector> sectors;
};

} // namespace srsran

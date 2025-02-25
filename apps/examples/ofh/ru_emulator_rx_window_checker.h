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

#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/srslog/logger.h"
#include <atomic>

namespace srsran {

/// Helper structure with aggregated reception window statistics.
struct ru_emulator_rx_kpis {
  uint64_t rx_on_time;
  uint64_t rx_late;
  uint64_t rx_early;
};

/// \brief Reception window checker.
///
/// Checks if the given slot and symbol is within the reception window or not and stores the statistics.
class ru_emulator_rx_window_checker : public ofh::ota_symbol_boundary_notifier
{
  /// Helper class that represents the reception window statistics.
  class rx_window_checker_statistics
  {
    std::atomic<uint64_t> on_time_counter{0};
    std::atomic<uint64_t> early_counter{0};
    std::atomic<uint64_t> late_counter{0};
    uint64_t              last_on_time_value_printed = 0U;
    uint64_t              last_early_value_printed   = 0U;
    uint64_t              last_late_value_printed    = 0U;

  public:
    /// Returns the statistics collected from the last time this function was called.
    ru_emulator_rx_kpis get_statistics();

    /// Functions to increment the counters.
    void increment_on_time_counter() { on_time_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_early_counter() { early_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_late_counter() { late_counter.fetch_add(1, std::memory_order_relaxed); }

    /// Getters to the message counters.
    uint64_t nof_on_time_messages() const { return on_time_counter.load(std::memory_order_relaxed); }
    uint64_t nof_early_messages() const { return early_counter.load(std::memory_order_relaxed); }
    uint64_t nof_late_messages() const { return late_counter.load(std::memory_order_relaxed); }
  };

  const ofh::rx_window_timing_parameters timing_parameters;
  rx_window_checker_statistics           statistics;
  std::atomic<uint32_t>                  count_val;

public:
  explicit ru_emulator_rx_window_checker(ofh::rx_window_timing_parameters params) : timing_parameters(params) {}

  // See interface for documentation.
  void on_new_symbol(const ofh::slot_symbol_point_context& symbol_point_context) override;

  /// \brief Updates the Rx window statistics.
  ///
  /// \param symbol_point slot and symbol of a received Open Fronthaul message.
  /// \return true if message was received on time, false otherwise.
  bool update_rx_window_statistics(ofh::slot_symbol_point symbol_point);

  /// Getters to the number of messages.
  uint64_t nof_on_time_messages() const { return statistics.nof_on_time_messages(); }
  uint64_t nof_early_messages() const { return statistics.nof_early_messages(); }
  uint64_t nof_late_messages() const { return statistics.nof_late_messages(); }

  /// Returns the statistics collected from the last time this function was called.
  ru_emulator_rx_kpis get_statistics();
};

} // namespace srsran

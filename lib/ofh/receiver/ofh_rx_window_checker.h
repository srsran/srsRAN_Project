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

#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul reception window checker.
///
/// Checks if the given slot and symbol is within the reception window or not. The window checker also collects
/// statistics and prints them every second.
class rx_window_checker : public ota_symbol_boundary_notifier
{
  /// Helper class that represents the reception window statistics.
  class rx_window_checker_statistics
  {
    static constexpr unsigned NOF_BITS_PER_COUNTER = 21U;

    srslog::basic_logger& logger;
    std::atomic<uint64_t> on_time_counter{0};
    std::atomic<uint64_t> early_counter{0};
    std::atomic<uint64_t> late_counter{0};
    uint64_t              last_on_time_value_printed = 0U;
    uint64_t              last_early_value_printed   = 0U;
    uint64_t              last_late_value_printed    = 0U;

  public:
    explicit rx_window_checker_statistics(srslog::basic_logger& logger_) : logger(logger_) {}

    /// Prints the statistics.
    void print_statistics();

    /// Functions to increment the counters.
    void increment_on_time_counter() { on_time_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_early_counter() { early_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_late_counter() { late_counter.fetch_add(1, std::memory_order_relaxed); }

    /// Getters to the message counters.
    uint64_t nof_on_time_messages() const { return on_time_counter.load(std::memory_order_relaxed); }
    uint64_t nof_early_messages() const { return early_counter.load(std::memory_order_relaxed); }
    uint64_t nof_late_messages() const { return late_counter.load(std::memory_order_relaxed); }
  };

  const rx_window_timing_parameters timing_parameters;
  const unsigned                    nof_symbols_in_one_second;
  const bool                        is_disabled;
  unsigned                          nof_symbols;
  rx_window_checker_statistics      statistics;
  std::atomic<uint32_t>             count_val;

public:
  rx_window_checker(srslog::basic_logger&                    logger_,
                    const rx_window_timing_parameters&       params,
                    std::chrono::duration<double, std::nano> symbol_duration);

  // See interface for documentation.
  void on_new_symbol(slot_symbol_point symbol_point) override;

  /// Returns true if the Rx window checker is disabled, otherwise returns false.
  bool disabled() const { return is_disabled; }

  /// Updates the Rx window statistics.
  void update_rx_window_statistics(slot_symbol_point symbol_point);

  /// Getters to the number of messages.
  uint64_t nof_on_time_messages() const { return statistics.nof_on_time_messages(); }
  uint64_t nof_early_messages() const { return statistics.nof_early_messages(); }
  uint64_t nof_late_messages() const { return statistics.nof_late_messages(); }

private:
  /// Returns true if the given logger is enabled for info level, otherwise false.
  static bool is_log_enabled(srslog::basic_logger& logger) { return logger.info.enabled(); }

  /// Prints the statistics every second.
  void print_statistics();
};

} // namespace ofh
} // namespace srsran

/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul reception window checker.
///
/// Checks if the given slot and symbol is within the reception window or not. The window checker also collects
/// statistics and prints them every second.
class rx_window_checker : public ota_symbol_handler
{
  /// Helper class that represents the reception window statistics.
  class rx_window_checker_statistics
  {
    static constexpr unsigned NOF_BITS_PER_COUNTER = 21U;

    std::atomic<uint64_t> on_time_counter{0};
    std::atomic<uint64_t> early_counter{0};
    std::atomic<uint64_t> late_counter{0};
    uint64_t              last_on_time_value_printed = 0U;
    uint64_t              last_early_value_printed   = 0U;
    uint64_t              last_late_value_printed    = 0U;

  public:
    /// Prints the statistics using the given logger.
    void print_statistics(srslog::basic_logger& logger);

    /// Functions to increment the counters.
    void increment_on_time_counter() { on_time_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_early_counter() { early_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_late_counter() { late_counter.fetch_add(1, std::memory_order_relaxed); }

    /// Getters to the message counters.
    uint64_t nof_on_time_messages() const { return on_time_counter.load(std::memory_order_relaxed); }
    uint64_t nof_early_messages() const { return early_counter.load(std::memory_order_relaxed); }
    uint64_t nof_late_messages() const { return late_counter.load(std::memory_order_relaxed); }
  };

  /// Reception window timing parameters.
  struct rx_timing_parameters {
    /// Offset from the current OTA symbol to the first symbol at which UL User-Plane message can be received within its
    /// reception window. Must be calculated based on \c Ta4_min parameter.
    int sym_start;
    /// Offset from the current OTA symbol to the last symbol at which UL User-Plane message can be received within its
    /// reception window. Must be calculated based on \c Ta4_max parameter.
    int sym_end;

    rx_timing_parameters(const du_rx_window_timing_parameters&    params,
                         std::chrono::duration<double, std::nano> symbol_duration) :
      sym_start(std::ceil(params.Ta4_min / symbol_duration)), sym_end(std::floor(params.Ta4_max / symbol_duration))
    {
    }
  };

public:
  rx_window_checker(srslog::basic_logger&                    logger_,
                    const du_rx_window_timing_parameters&    params,
                    std::chrono::duration<double, std::nano> symbol_duration);

  // See interface for documentation.
  void handle_new_ota_symbol(slot_symbol_point symbol_point) override;

  /// Returns true if the given symbol point is within the reception window, otherwise false.
  bool update_rx_window_statistics(slot_symbol_point symbol_point);

  /// Getters to the number of messages.
  uint64_t nof_on_time_messages() const { return statistics.nof_on_time_messages(); }
  uint64_t nof_early_messages() const { return statistics.nof_early_messages(); }
  uint64_t nof_late_messages() const { return statistics.nof_late_messages(); }

private:
  /// Prints the statistics every second.
  void print_statistics();

private:
  srslog::basic_logger&        logger;
  const rx_timing_parameters   timing_parameters;
  const unsigned               nof_symbols_in_one_second;
  unsigned                     nof_symbols;
  rx_window_checker_statistics statistics;
  std::atomic<uint32_t>        count_val;
};

} // namespace ofh
} // namespace srsran

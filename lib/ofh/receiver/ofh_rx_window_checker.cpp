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

#include "ofh_rx_window_checker.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"

using namespace srsran;
using namespace ofh;

static constexpr unsigned OFH_MAX_NOF_SFN = 256U;

rx_window_checker::rx_window_checker(bool is_enabled, const rx_window_timing_parameters& params) :
  timing_parameters(params), is_disabled(!is_enabled), slot_raw_value(0)
{
}

/// Calculates the Open Fronthaul slot symbol point for the given symbol point. An Open Fronthaul slot symbol point is a
/// normal one, but with the SFN values restricted to 1 Byte.
static slot_symbol_point calculate_ofh_slot_symbol_point(slot_symbol_point symbol_point)
{
  // Build a new slot symbol point that manages that the SFN values in OFH is 1 byte.
  slot_point ota_slot = symbol_point.get_slot();
  slot_point ofh_slot(ota_slot.numerology(), ota_slot.sfn() % OFH_MAX_NOF_SFN, ota_slot.slot_index());

  return {ofh_slot, symbol_point.get_symbol_index(), symbol_point.get_nof_symbols()};
}

/// Calculate the distance between the given slot symbol points in symbols.
static int calculate_slot_symbol_point_distance(slot_symbol_point lhs, slot_symbol_point rhs)
{
  srsran_assert(rhs.get_numerology() == lhs.get_numerology(),
                "Cannot calculate the distance of two slot symbol points that have different numerologies");
  srsran_assert(rhs.get_nof_symbols() == lhs.get_nof_symbols(),
                "Cannot calculate the distance of two slot symbol points that have a different number of symbols");

  const int nof_symbols_per_slot_wrap = OFH_MAX_NOF_SFN * NOF_SUBFRAMES_PER_FRAME *
                                        get_nof_slots_per_subframe(to_subcarrier_spacing(rhs.get_numerology())) *
                                        rhs.get_nof_symbols();

  int a = static_cast<int>(lhs.to_uint()) - static_cast<int>(rhs.to_uint());
  if (a >= nof_symbols_per_slot_wrap / 2) {
    return a - nof_symbols_per_slot_wrap;
  }
  if (a < -nof_symbols_per_slot_wrap / 2) {
    return a + nof_symbols_per_slot_wrap;
  }
  return a;
}

void rx_window_checker::on_new_symbol(const slot_symbol_point_context& symbol_point_context)
{
  if (is_disabled) {
    return;
  }

  // Build a new slot symbol point that manages that the SFN values in OFH is 1 byte.
  slot_symbol_point ota_symbol_point = calculate_ofh_slot_symbol_point(symbol_point_context.symbol_point);

  // Update the stored slot symbol point as system value.
  slot_raw_value.store(ota_symbol_point.to_uint(), std::memory_order_relaxed);
}

void rx_window_checker::update_rx_window_statistics(slot_symbol_point symbol_point)
{
  if (is_disabled) {
    return;
  }

  // Store the ota symbol point to use the same value for the early and late points.
  slot_symbol_point ota_point(
      symbol_point.get_numerology(), slot_raw_value.load(std::memory_order_relaxed), symbol_point.get_nof_symbols());

  // Calculate the distance between the 2 slot symbol points in symbols.
  int diff = calculate_slot_symbol_point_distance(ota_point, symbol_point);

  // Late detected.
  if (diff > static_cast<int>(timing_parameters.sym_end)) {
    statistics.increment_late_counter();
    return;
  }

  // Early detected.
  if (diff < static_cast<int>(timing_parameters.sym_start)) {
    statistics.increment_early_counter();
    return;
  }

  // On time detected.
  statistics.increment_on_time_counter();
}

void rx_window_checker::collect_metrics(received_messages_metrics& metrics)
{
  statistics.collect_metrics(metrics);
}

void rx_window_checker::rx_window_checker_statistics::collect_metrics(received_messages_metrics& metrics)
{
  // Fetch the data.
  uint64_t current_nof_on_time = nof_on_time_messages();
  uint64_t current_nof_late    = nof_late_messages();
  uint64_t current_nof_early   = nof_early_messages();

  // Calculate the difference since last print.
  metrics.nof_on_time_messages = current_nof_on_time - last_on_time_value_printed;
  metrics.nof_early_messages   = current_nof_early - last_early_value_printed;
  metrics.nof_late_messages    = current_nof_late - last_late_value_printed;

  // Update last print.
  last_late_value_printed    = current_nof_late;
  last_early_value_printed   = current_nof_early;
  last_on_time_value_printed = current_nof_on_time;
}

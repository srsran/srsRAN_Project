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

#include "ru_ofh_timing_notifier_impl.h"
#include "srsran/ru/ru_timing_notifier.h"
#include <chrono>

using namespace srsran;

ru_ofh_timing_notifier_impl::ru_ofh_timing_notifier_impl(unsigned            nof_slot_offset_du_ru_,
                                                         unsigned            nof_symbols_per_slot,
                                                         subcarrier_spacing  scs,
                                                         ru_timing_notifier& timing_notifier_) :
  nof_slot_offset_du_ru(nof_slot_offset_du_ru_),
  nof_slots_offset_du_ru_ns(nof_slot_offset_du_ru * 1000000 / slot_point(scs, 0).nof_slots_per_subframe()),
  half_slot_symbol(nof_symbols_per_slot / 2U - 1U),
  full_slot_symbol(nof_symbols_per_slot - 1U),
  timing_notifier(timing_notifier_)
{
}

void ru_ofh_timing_notifier_impl::on_new_symbol(const ofh::slot_symbol_point_context& symbol_point_context)
{
  if (symbol_point_context.symbol_point.get_symbol_index() == half_slot_symbol) {
    timing_notifier.on_ul_half_slot_boundary(symbol_point_context.symbol_point.get_slot());
  }

  if (symbol_point_context.symbol_point.get_symbol_index() == full_slot_symbol) {
    timing_notifier.on_ul_full_slot_boundary(symbol_point_context.symbol_point.get_slot());
  }

  // New slots start on symbol index 0.
  if (symbol_point_context.symbol_point.get_symbol_index() == 0) {
    // Create the context.
    tti_boundary_context context;
    context.slot       = symbol_point_context.symbol_point.get_slot() + nof_slot_offset_du_ru;
    context.time_point = symbol_point_context.time_point + nof_slots_offset_du_ru_ns;

    timing_notifier.on_tti_boundary(context);
  }
}

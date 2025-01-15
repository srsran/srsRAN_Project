/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_timing_notifier_impl.h"
#include "srsran/ru/ru_timing_notifier.h"

using namespace srsran;

void ru_ofh_timing_notifier_impl::on_new_symbol(ofh::slot_symbol_point symbol_point)
{
  if (symbol_point.get_symbol_index() == half_slot_symbol) {
    timing_notifier.on_ul_half_slot_boundary(symbol_point.get_slot());
  }

  if (symbol_point.get_symbol_index() == full_slot_symbol) {
    timing_notifier.on_ul_full_slot_boundary(symbol_point.get_slot());
  }

  // New slots start on symbol index 0.
  if (symbol_point.get_symbol_index() == 0) {
    timing_notifier.on_tti_boundary(symbol_point.get_slot() + nof_slot_offset_du_ru);
  }
}

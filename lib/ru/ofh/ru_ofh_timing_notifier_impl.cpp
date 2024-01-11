/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_timing_notifier_impl.h"
#include "srsran/ru/ru_timing_notifier.h"

using namespace srsran;

void ru_ofh_timing_notifier_impl::notify_new_slot(ofh::slot_symbol_point symbol_point)
{
  // First incoming slot, update the current slot and exit. Exiting in this point will make to notify to the upper
  // layers when a new slot starts, instead of notifying in a random time point of the slot.
  if (!current_slot.valid()) {
    current_slot = symbol_point.get_slot();

    return;
  }

  // Skip if the slot did not change.
  if (symbol_point.get_slot() == current_slot) {
    return;
  }

  current_slot = symbol_point.get_slot();
  timing_notifier.on_tti_boundary(current_slot + nof_slot_offset_du_ru);
}

void ru_ofh_timing_notifier_impl::on_new_symbol(ofh::slot_symbol_point symbol_point)
{
  // First task, notify the new slot.
  notify_new_slot(symbol_point);

  if (symbol_point.get_symbol_index() == half_slot_symbol) {
    timing_notifier.on_ul_half_slot_boundary(symbol_point.get_slot());
  }

  if (symbol_point.get_symbol_index() == full_slot_symbol) {
    timing_notifier.on_ul_full_slot_boundary(symbol_point.get_slot());
  }
}

ru_ofh_timing_notifier_impl::ru_ofh_timing_notifier_impl(unsigned            nof_slot_offset_du_ru_,
                                                         unsigned            nof_symbols_per_slot,
                                                         ru_timing_notifier& timing_notifier_) :
  nof_slot_offset_du_ru(nof_slot_offset_du_ru_),
  half_slot_symbol(nof_symbols_per_slot / 2U - 1U),
  full_slot_symbol(nof_symbols_per_slot - 1U),
  timing_notifier(timing_notifier_)
{
}

/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_ota_symbol_dispatcher.h"

using namespace srsran;
using namespace ofh;

void ota_symbol_dispatcher::on_new_symbol(slot_symbol_point symbol_point)
{
  // Handle the new symbol.
  for (auto* handler : symbol_handlers) {
    handler->handle_new_ota_symbol(symbol_point);
  }

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

  // At this point, notify the change of slot.
  if ((current_slot + 1) != symbol_point.get_slot()) {
    logger.warning(
        "Detected a non-consecutive slot change. current_slot={} , new_slot={}", current_slot, symbol_point.get_slot());
  }

  if (symbol_point.get_symbol_index() == half_slot_symbol) {
    time_notifier->on_ul_half_slot_boundary(symbol_point.get_slot());
  }

  if (symbol_point.get_symbol_index() == full_slot_symbol) {
    time_notifier->on_ul_full_slot_boundary(symbol_point.get_slot());
  }

  current_slot = symbol_point.get_slot();
  time_notifier->on_tti_boundary(current_slot + nof_slot_offset_du_ru);
}

ota_symbol_dispatcher::ota_symbol_dispatcher(unsigned                         nof_slot_offset_du_ru_,
                                             unsigned                         nof_symbols_per_slot,
                                             srslog::basic_logger&            logger_,
                                             std::unique_ptr<timing_notifier> timing_notifier_,
                                             span<ota_symbol_handler*>        symbol_handlers_) :
  nof_slot_offset_du_ru(nof_slot_offset_du_ru_),
  half_slot_symbol(nof_symbols_per_slot / 2U - 1U),
  full_slot_symbol(nof_symbols_per_slot - 1U),
  logger(logger_),
  time_notifier(std::move(timing_notifier_)),
  symbol_handlers(symbol_handlers_.begin(), symbol_handlers_.end())
{
}

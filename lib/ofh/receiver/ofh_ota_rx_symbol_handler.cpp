/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_ota_rx_symbol_handler.h"

using namespace srsran;
using namespace ofh;

/// Number of slots after the OTA when the repositories will be cleared.
static constexpr unsigned delay_slots_to_clear = 10U;

void ota_rx_symbol_handler::on_new_symbol(slot_symbol_point symbol_point)
{
  if (!current_slot.valid()) {
    current_slot = symbol_point.get_slot();
    clear_slot_in_repositories(current_slot);
    return;
  }

  // Same slot, do nothing.
  if (symbol_point.get_slot() == current_slot) {
    return;
  }

  current_slot = symbol_point.get_slot();
  clear_slot_in_repositories(current_slot + delay_slots_to_clear);
}

void ota_rx_symbol_handler::clear_slot_in_repositories(slot_point slot)
{
  cp_context_repo.clear_slot(slot);
  prach_repo.clear(slot);
  ul_slot_repo.clear(slot);
}

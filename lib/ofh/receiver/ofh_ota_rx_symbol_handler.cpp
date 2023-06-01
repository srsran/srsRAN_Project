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

#include "ofh_ota_rx_symbol_handler.h"

using namespace srsran;
using namespace ofh;

/// Number of slots after the OTA when the repositories will be cleared.
static constexpr unsigned delay_slots_to_clear = 10U;

void ota_rx_symbol_handler::on_new_symbol(slot_point slot, unsigned symbol_index)
{
  if (!current_slot.valid()) {
    current_slot = slot;
    clear_slot_in_repositories(slot);

    return;
  }

  // Same slot, do nothing.
  if (slot == current_slot) {
    return;
  }

  current_slot = slot;
  clear_slot_in_repositories(slot + delay_slots_to_clear);
}

void ota_rx_symbol_handler::clear_slot_in_repositories(slot_point slot)
{
  cp_context_repo.clear_slot(slot);
  prach_repo.clear(slot);
  ul_slot_repo.clear(slot);
}

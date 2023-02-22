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

#include "srsran/scheduler/scheduler_feedback_handler.h"

namespace srsran {

rach_indication_message::preamble create_preamble(unsigned preamble_id, rnti_t tc_rnti)
{
  rach_indication_message::preamble preamble{};
  preamble.preamble_id  = preamble_id;
  preamble.time_advance = phy_time_unit::from_seconds(0);
  preamble.tc_rnti      = tc_rnti;
  return preamble;
}

rach_indication_message create_rach_indication(slot_point                                               next_slot_rx,
                                               std::initializer_list<rach_indication_message::preamble> preambles)
{
  rach_indication_message rach_ind{};
  rach_ind.cell_index = to_du_cell_index(0);
  rach_ind.slot_rx    = next_slot_rx;
  if (preambles.size() == 0) {
    return rach_ind;
  }
  rach_ind.occasions.emplace_back();
  rach_ind.occasions.back().start_symbol    = 0;
  rach_ind.occasions.back().frequency_index = 0;

  for (const auto& preamble : preambles) {
    rach_ind.occasions.back().preambles.push_back(preamble);
  }
  return rach_ind;
}

} // namespace srsran
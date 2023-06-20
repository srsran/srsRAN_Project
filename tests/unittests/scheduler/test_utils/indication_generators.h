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

inline rach_indication_message::preamble create_preamble(unsigned preamble_id, rnti_t tc_rnti)
{
  rach_indication_message::preamble preamble{};
  preamble.preamble_id  = preamble_id;
  preamble.time_advance = phy_time_unit::from_seconds(0);
  preamble.tc_rnti      = tc_rnti;
  return preamble;
}

inline rach_indication_message
create_rach_indication(slot_point next_slot_rx, std::initializer_list<rach_indication_message::preamble> preambles)
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

inline uci_indication create_uci_indication(slot_point uci_sl, du_ue_index_t ue_idx, const pucch_info& pucch_pdu)
{
  uci_indication uci_ind{};
  uci_ind.cell_index = to_du_cell_index(0);
  uci_ind.slot_rx    = uci_sl;
  uci_ind.ucis.resize(1);
  uci_ind.ucis[0].crnti    = pucch_pdu.crnti;
  uci_ind.ucis[0].ue_index = ue_idx;
  switch (pucch_pdu.format) {
    case pucch_format::FORMAT_1: {
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f1{};
      f1.harqs.resize(pucch_pdu.format_1.harq_ack_nof_bits);
      std::fill(f1.harqs.begin(), f1.harqs.end(), mac_harq_ack_report_status::ack);
      uci_ind.ucis[0].pdu = f1;
    } break;
    case pucch_format::FORMAT_2: {
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2{};
      f2.harqs.resize(pucch_pdu.format_2.harq_ack_nof_bits);
      std::fill(f2.harqs.begin(), f2.harqs.end(), mac_harq_ack_report_status::ack);
      uci_ind.ucis[0].pdu = f2;
      if (pucch_pdu.format_2.csi_part1_bits > 0) {
        f2.csi.emplace();
        f2.csi->first_tb_wideband_cqi = 15;
      }
    } break;
    default:
      report_fatal_error("Unsupported PUCCH format");
  }
  return uci_ind;
}

} // namespace srsran
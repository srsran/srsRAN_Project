/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "mac_test_messages.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

using namespace srsran;

mac_rx_data_indication srsran::test_helpers::create_ccch_message(slot_point sl_rx, rnti_t rnti)
{
  return mac_rx_data_indication{
      sl_rx,
      to_du_cell_index(0),
      {mac_rx_pdu{rnti,
                  0,
                  0,
                  byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}).value()}}};
}

mac_rx_data_indication srsran::test_helpers::create_pdu_with_sdu(slot_point sl_rx, rnti_t rnti, lcid_t lcid)
{
  return mac_rx_data_indication{
      sl_rx,
      to_du_cell_index(0),
      {mac_rx_pdu{
          rnti,
          0,
          0,
          byte_buffer::create({(uint8_t)lcid, 0x23, 0xc0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x05, 0xdf, 0x80, 0x10, 0x5e,
                               0x40,          0x03, 0x40, 0x40, 0x3c, 0x44, 0x3c, 0x3f, 0xc0, 0x00, 0x04, 0x0c, 0x95,
                               0x1d,          0xa6, 0x0b, 0x80, 0xb8, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00})
              .value()}}};
}

mac_uci_pdu srsran::test_helpers::create_uci_pdu(const pucch_info& pucch)
{
  mac_uci_pdu pdu{};

  pdu.rnti = pucch.crnti;

  switch (pucch.format) {
    case pucch_format::FORMAT_1: {
      auto&       uci_f1   = pdu.pdu.emplace<mac_uci_pdu::pucch_f0_or_f1_type>();
      const auto& pucch_f1 = pucch.format_1;

      if (pucch_f1.harq_ack_nof_bits > 0) {
        uci_f1.harq_info.emplace();
        uci_f1.harq_info->harqs.resize(pucch_f1.harq_ack_nof_bits, uci_pucch_f0_or_f1_harq_values::ack);
      }

      if (pucch_f1.sr_bits != sr_nof_bits::no_sr) {
        uci_f1.sr_info.emplace();
        uci_f1.sr_info->detected = false;
      }
    } break;
    case pucch_format::FORMAT_2: {
      auto&       uci_f2   = pdu.pdu.emplace<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>();
      const auto& pucch_f2 = pucch.format_2;

      if (pucch_f2.harq_ack_nof_bits > 0) {
        uci_f2.harq_info.emplace();
        uci_f2.harq_info->is_valid = true;
        uci_f2.harq_info->payload.resize(pucch_f2.harq_ack_nof_bits);
        uci_f2.harq_info->payload.fill(true);
      }

      if (pucch_f2.sr_bits != sr_nof_bits::no_sr) {
        uci_f2.sr_info.emplace();
        uci_f2.sr_info->resize(sr_nof_bits_to_uint(pucch_f2.sr_bits));
      }

      if (pucch_f2.csi_part1_bits > 0) {
        uci_f2.csi_part1_info.emplace();
        uci_f2.csi_part1_info->is_valid = true;
        uci_f2.csi_part1_info->payload.resize(pucch_f2.csi_part1_bits);
        uci_f2.csi_part1_info->payload.fill(true);
      }
    } break;
    default:
      report_fatal_error("Unsupported PUCCH format");
  }

  return pdu;
}

mac_uci_indication_message srsran::test_helpers::create_uci_indication(slot_point sl_rx, span<const pucch_info> pucchs)
{
  mac_uci_indication_message uci_ind;
  uci_ind.sl_rx = sl_rx;
  for (const auto& pucch : pucchs) {
    uci_ind.ucis.push_back(create_uci_pdu(pucch));
  }
  return uci_ind;
}

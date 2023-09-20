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

#include "uci_cell_decoder.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"

using namespace srsran;

uci_cell_decoder::uci_cell_decoder(const sched_cell_configuration_request_message& cell_cfg,
                                   const du_rnti_table&                            rnti_table_,
                                   rlf_detector&                                   rlf_hdlr_) :
  rnti_table(rnti_table_),
  cell_index(cell_cfg.cell_index),
  rlf_handler(rlf_hdlr_),
  logger(srslog::fetch_basic_logger("MAC"))
{
}

static optional<csi_report_data>
decode_csi(uci_pusch_or_pucch_f2_3_4_detection_status                            csi_status,
           const bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& csi1_bits,
           const optional<uint8_t>&                                              ri,
           const optional<uint8_t>&                                              pmi)
{
  optional<csi_report_data> rep;

  if (csi_status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
    rep.emplace();

    // Convert CSI-1 bits to CSI report.
    // TODO: Support more CSI encodings.

    // Refer to \ref mac_uci_pdu::pucch_f2_or_f3_or_f4_type::uci_payload_or_csi_information for the CSI payload bit
    // encoding.
    unsigned wb_cqi = (static_cast<unsigned>(csi1_bits.test(0)) << 3) +
                      (static_cast<unsigned>(csi1_bits.test(1)) << 2) +
                      (static_cast<unsigned>(csi1_bits.test(2)) << 1) + (static_cast<unsigned>(csi1_bits.test(3)));
    rep->first_tb_wideband_cqi = wb_cqi;

    if (ri.has_value()) {
      rep->ri = csi_report_data::ri_type{*ri}; // TODO: Support RI decoding.
    }

    if (pmi.has_value()) {
      rep->pmi.emplace();
      rep->pmi->type = csi_report_pmi::two_antenna_port{*pmi}; // TODO: Support PMI decoding.
    }
  }

  return rep;
}

static auto convert_mac_harq_bits_to_sched_harq_values(uci_pusch_or_pucch_f2_3_4_detection_status harq_status,
                                                       const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
{
  bool crc_pass = harq_status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
  static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> ret(
      payload.size(), crc_pass ? mac_harq_ack_report_status::nack : mac_harq_ack_report_status::dtx);
  if (crc_pass) {
    for (unsigned i = 0, e = ret.size(); i != e; ++i) {
      if (payload.test(i)) {
        ret[i] = srsran::mac_harq_ack_report_status::ack;
      }
    }
  }
  return ret;
}

static csi_report_data decode_csi_bits(const mac_uci_pdu::pucch_f2_or_f3_or_f4_type& pucch,
                                       const csi_report_configuration&               csi_rep_cfg)
{
  // Convert UCI CSI1 bits to "csi_report_packed".
  csi_report_packed csi_bits(pucch.uci_part1_or_csi_part1_info->payload.size());
  for (unsigned k = 0; k != csi_bits.size(); ++k) {
    csi_bits.set(k, pucch.uci_part1_or_csi_part1_info->payload.test(k));
  }

  return csi_report_unpack_pucch(csi_bits, csi_rep_cfg);
}

uci_indication uci_cell_decoder::decode_uci(const mac_uci_indication_message& msg)
{
  // Convert MAC UCI indication to srsRAN scheduler UCI indication.
  uci_indication ind{};
  ind.slot_rx    = msg.sl_rx;
  ind.cell_index = cell_index;
  for (unsigned i = 0; i != msg.ucis.size(); ++i) {
    uci_indication::uci_pdu& uci_pdu = ind.ucis.emplace_back();
    uci_pdu.crnti                    = msg.ucis[i].rnti;
    uci_pdu.ue_index                 = rnti_table[msg.ucis[i].rnti];
    if (ind.ucis[i].ue_index == INVALID_DU_UE_INDEX) {
      ind.ucis.pop_back();
      logger.info("rnti={}: Discarding UCI PDU. Cause: No UE with provided RNTI exists.", uci_pdu.crnti);
      continue;
    }

    if (variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(msg.ucis[i].pdu)) {
      const auto& pucch = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(msg.ucis[i].pdu);

      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pdu{};
      if (pucch.ul_sinr.has_value()) {
        pdu.ul_sinr.emplace(pucch.ul_sinr.value());
      }
      if (pucch.time_advance_offset.has_value()) {
        pdu.time_advance_offset.emplace(pucch.time_advance_offset.value());
      }
      pdu.sr_detected = false;
      if (pucch.sr_info.has_value()) {
        pdu.sr_detected = pucch.sr_info.value().sr_detected;
      }
      if (pucch.harq_info.has_value()) {
        // NOTES:
        // - We report to the scheduler only the UCI HARQ-ACKs that contain either an ACK or NACK; we ignore the
        // UCIs with DTX. In that case, the scheduler will not receive the notification and the HARQ will eventually
        // retransmit the packet.
        // - This is to handle the case of simultaneous SR + HARQ UCI, for which we receive 2 UCI PDUs from the PHY,
        // 1 for SR + HARQ, 1 for HARQ only; note that only the SR + HARQ UCI is filled by the UE, meaning that we
        // expect the received HARQ-only UCI to be DTX. If reported to the scheduler, the UCI with HARQ-ACK only would
        // be erroneously treated as a NACK (as the scheduler only accepts ACK or NACK).

        // NOTE: There is a potential error that need to be handled below, which occurs when there's the 2-bit report
        // {DTX, (N)ACK}; if this were reported, we would skip the first bit (i.e. DTX) and report the second (i.e.
        // (N)ACK). Since in the scheduler the HARQ-ACK bits for a given UCI are processed in sequence, the
        // notification of the second bit of {DTX, (N)ACK} would be seen by the scheduler as the first bit of the
        // expected 2-bit reporting. To prevent this, we assume that PUCCH Format 0 or 1 UCI is valid if none of the 1
        // or 2 bits report is DTX (not detected).

        const auto& harq_pdus = pucch.harq_info.value().harqs;
        pdu.harqs.resize(harq_pdus.size());
        for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
          switch (harq_pdus[j]) {
            case uci_pucch_f0_or_f1_harq_values::ack:
              pdu.harqs[j] = mac_harq_ack_report_status::ack;
              break;
            case uci_pucch_f0_or_f1_harq_values::nack:
              pdu.harqs[j] = mac_harq_ack_report_status::nack;
              break;
            default:
              pdu.harqs[j] = mac_harq_ack_report_status::dtx;
          }

          // Report ACK for RLF detection purposes.
          rlf_handler.handle_ack(ind.ucis[i].ue_index, pdu.harqs[j] == mac_harq_ack_report_status::ack);
        }
      }
      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(pdu);
    } else if (variant_holds_alternative<mac_uci_pdu::pusch_type>(msg.ucis[i].pdu)) {
      const auto&                            pusch = variant_get<mac_uci_pdu::pusch_type>(msg.ucis[i].pdu);
      uci_indication::uci_pdu::uci_pusch_pdu pdu{};
      if (pusch.harq_info.has_value()) {
        pdu.harqs =
            convert_mac_harq_bits_to_sched_harq_values(pusch.harq_info.value().harq_status, pusch.harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
          rlf_handler.handle_ack(ind.ucis[i].ue_index, pdu.harqs[j] == mac_harq_ack_report_status::ack);
        }
      }

      if (pusch.csi_part1_info.has_value()) {
        pdu.csi = decode_csi(pusch.csi_part1_info->csi_status, pusch.csi_part1_info->payload, pusch.ri, pusch.pmi);
      }

      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pusch_pdu>(pdu);

    } else if (variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(msg.ucis[i].pdu)) {
      const auto& pucch = variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(msg.ucis[i].pdu);
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu pdu{};

      if (pucch.ul_sinr.has_value()) {
        pdu.ul_sinr.emplace(pucch.ul_sinr.value());
      }
      if (pucch.time_advance_offset.has_value()) {
        pdu.time_advance_offset.emplace(pucch.time_advance_offset.value());
      }
      if (pucch.sr_info.has_value()) {
        pdu.sr_info = pucch.sr_info.value();
      }
      if (pucch.harq_info.has_value()) {
        pdu.harqs =
            convert_mac_harq_bits_to_sched_harq_values(pucch.harq_info.value().harq_status, pucch.harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (const mac_harq_ack_report_status& harq_st : pdu.harqs) {
          rlf_handler.handle_ack(ind.ucis[i].ue_index, harq_st == mac_harq_ack_report_status::ack);
        }
      }

      // Check if the UCI has been correctly decoded.
      if (pucch.uci_part1_or_csi_part1_info.has_value() and
          pucch.uci_part1_or_csi_part1_info->status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
        // Decode CSI bits given the CSI report config previously stored in the grid.
        const auto& slot_ucis = expected_uci_report_grid[to_grid_index(msg.sl_rx)];

        // Search for CSI report config with matching RNTI.
        for (const auto& expected_slot_uci : slot_ucis) {
          if (expected_slot_uci.rnti == uci_pdu.crnti) {
            pdu.csi = decode_csi_bits(pucch, expected_slot_uci.csi_rep_cfg);
            break;
          }
        }
        if (not pdu.csi.has_value()) {
          logger.warning("cell={} ue={} rnti={}: Discarding CSI report. Cause: Unable to find CSI report config.",
                         cell_index,
                         uci_pdu.ue_index,
                         uci_pdu.crnti);
        }
      }

      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(pdu);
    }
  }

  return ind;
}

void uci_cell_decoder::store_uci(slot_point uci_sl, span<const pucch_info> scheduled_pucchs)
{
  auto& slot_ucis = expected_uci_report_grid[to_grid_index(uci_sl)];
  slot_ucis.clear();

  for (const pucch_info& pucch : scheduled_pucchs) {
    if (pucch.csi_rep_cfg.has_value()) {
      uci_context& uci_ctx = slot_ucis.emplace_back();
      uci_ctx.rnti         = pucch.crnti;
      uci_ctx.csi_rep_cfg  = *pucch.csi_rep_cfg;
    }
  }
}

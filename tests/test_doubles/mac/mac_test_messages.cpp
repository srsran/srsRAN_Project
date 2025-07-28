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

#include "mac_test_messages.h"
#include "srsran/scheduler/result/pucch_info.h"
#include "srsran/scheduler/result/pusch_info.h"
#include "srsran/scheduler/result/srs_info.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

mac_rx_data_indication
srsran::test_helpers::create_ccch_message(slot_point sl_rx, rnti_t rnti, du_cell_index_t du_cell_index)
{
  return mac_rx_data_indication{
      sl_rx,
      du_cell_index,
      {mac_rx_pdu{rnti,
                  0,
                  0,
                  byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}).value()}}};
}

mac_rx_data_indication
srsran::test_helpers::create_pdu_with_sdu(slot_point sl_rx, rnti_t rnti, lcid_t lcid, uint32_t rlc_sn)
{
  // assuming RLC SN size of 12bits.
  uint8_t rlc_sn_part1 = (rlc_sn & 0xf00U) >> 8U;
  uint8_t rlc_sn_part2 = (rlc_sn & 0xffU);

  byte_buffer mac_pdu = byte_buffer::create({(uint8_t)lcid, 0x23, static_cast<uint8_t>(0xc0U + rlc_sn_part1),
                                             rlc_sn_part2,  0x00, 0x00,
                                             0x10,          0x00, 0x05,
                                             0xdf,          0x80, 0x10,
                                             0x5e,          0x40, 0x03,
                                             0x40,          0x40, 0x3c,
                                             0x44,          0x3c, 0x3f,
                                             0xc0,          0x00, 0x04,
                                             0x0c,          0x95, 0x1d,
                                             0xa6,          0x0b, 0x80,
                                             0xb8,          0x38, 0x00,
                                             0x00,          0x00, 0x00,
                                             0x00})
                            .value();
  return mac_rx_data_indication{sl_rx, to_du_cell_index(0), {mac_rx_pdu{rnti, 0, 0, std::move(mac_pdu)}}};
}

mac_crc_indication_message srsran::test_helpers::create_crc_indication(slot_point sl_rx, rnti_t rnti, harq_id_t h_id)
{
  return mac_crc_indication_message{.sl_rx = sl_rx,
                                    .crcs  = {mac_crc_pdu{.rnti = rnti, .harq_id = h_id, .tb_crc_success = true}}};
}

mac_crc_indication_message srsran::test_helpers::create_crc_indication(slot_point                sl_rx,
                                                                       span<const ul_sched_info> puschs)
{
  mac_crc_indication_message crc_ind;
  crc_ind.sl_rx = sl_rx;
  crc_ind.crcs.resize(puschs.size());
  for (unsigned i = 0; i != puschs.size(); ++i) {
    crc_ind.crcs[i].rnti           = puschs[i].pusch_cfg.rnti;
    crc_ind.crcs[i].harq_id        = puschs[i].pusch_cfg.harq_id;
    crc_ind.crcs[i].tb_crc_success = true;
  }
  return crc_ind;
}

mac_uci_pdu srsran::test_helpers::create_uci_pdu(const pucch_info& pucch)
{
  mac_uci_pdu pdu{};

  pdu.rnti = pucch.crnti;

  switch (pucch.format()) {
    case pucch_format::FORMAT_1: {
      auto& uci_f1 = pdu.pdu.emplace<mac_uci_pdu::pucch_f0_or_f1_type>();

      if (pucch.uci_bits.harq_ack_nof_bits > 0) {
        uci_f1.harq_info.emplace();
        uci_f1.harq_info->harqs.resize(pucch.uci_bits.harq_ack_nof_bits, uci_pucch_f0_or_f1_harq_values::ack);
      }

      if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
        uci_f1.sr_info.emplace();
        uci_f1.sr_info->detected = true;
      }
    } break;
    case pucch_format::FORMAT_2: {
      auto& uci_f2 = pdu.pdu.emplace<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>();

      if (pucch.uci_bits.harq_ack_nof_bits > 0) {
        uci_f2.harq_info.emplace();
        uci_f2.harq_info->is_valid = true;
        uci_f2.harq_info->payload.resize(pucch.uci_bits.harq_ack_nof_bits);
        uci_f2.harq_info->payload.fill(true);
      }

      if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
        uci_f2.sr_info.emplace();
        uci_f2.sr_info->resize(sr_nof_bits_to_uint(pucch.uci_bits.sr_bits));
      }

      if (pucch.uci_bits.csi_part1_nof_bits > 0) {
        uci_f2.csi_part1_info.emplace();
        uci_f2.csi_part1_info->is_valid = true;
        uci_f2.csi_part1_info->payload.resize(pucch.uci_bits.csi_part1_nof_bits);
        uci_f2.csi_part1_info->payload.fill(true);
      }
    } break;
    default:
      report_fatal_error("Unsupported PUCCH format");
  }

  return pdu;
}

mac_uci_pdu srsran::test_helpers::create_uci_pdu(rnti_t rnti, const uci_info& pusch_uci)
{
  mac_uci_pdu pdu{};
  pdu.rnti             = rnti;
  auto& pusch_ind      = pdu.pdu.emplace<mac_uci_pdu::pusch_type>();
  pusch_ind.ul_sinr_dB = 100;

  if (pusch_uci.harq.has_value() and pusch_uci.harq->harq_ack_nof_bits > 0) {
    auto& harq = pusch_uci.harq.value();
    pusch_ind.harq_info.emplace();
    pusch_ind.harq_info->is_valid = true;
    pusch_ind.harq_info->payload.resize(harq.harq_ack_nof_bits);
    pusch_ind.harq_info->payload.fill(true);
  }
  if (pusch_uci.csi.has_value() and pusch_uci.csi->csi_part1_nof_bits > 0) {
    pusch_ind.csi_part1_info.emplace();
    pusch_ind.csi_part1_info->is_valid = true;
    pusch_ind.csi_part1_info->payload.resize(pusch_uci.csi->csi_part1_nof_bits);
    pusch_ind.csi_part1_info->payload.fill(true);
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

std::optional<mac_uci_indication_message> srsran::test_helpers::create_uci_indication(slot_point                sl_rx,
                                                                                      span<const ul_sched_info> puschs)
{
  mac_uci_indication_message uci_ind;
  uci_ind.sl_rx = sl_rx;
  for (const auto& pusch : puschs) {
    if (pusch.uci.has_value()) {
      uci_ind.ucis.push_back(create_uci_pdu(pusch.pusch_cfg.rnti, pusch.uci.value()));
    }
  }
  return uci_ind.ucis.empty() ? std::nullopt : std::make_optional(uci_ind);
}

mac_srs_pdu test_helpers::create_srs_pdu(const srs_info& srs)
{
  phy_time_unit ta = phy_time_unit::from_timing_advance(test_rgen::uniform_int<unsigned>(0, 60), srs.bwp_cfg->scs);
  if (srs.positioning_report_requested) {
    // Set a random number, just to test the values are passed to the positioning report.
    phy_time_unit rtoa = phy_time_unit::from_units_of_Tc(test_rgen::uniform_int<unsigned>(0, 1000));
    const float   rsrp = -84.6f;
    return mac_srs_pdu(srs.crnti, ta, rtoa, rsrp);
  } else {
    srs_channel_matrix ch_matrix(1, 1);
    return mac_srs_pdu(srs.crnti, ta, ch_matrix);
  }
}

mac_srs_indication_message test_helpers::create_srs_indication(slot_point sl_rx, span<const srs_info> srss)
{
  mac_srs_indication_message srs_ind;
  srs_ind.sl_rx = sl_rx;
  for (const auto& srs : srss) {
    srs_ind.srss.push_back(create_srs_pdu(srs));
  }
  return srs_ind;
}

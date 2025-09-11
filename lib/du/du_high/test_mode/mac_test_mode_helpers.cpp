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

#include "mac_test_mode_helpers.h"
#include "srsran/scheduler/result/pucch_info.h"
#include "srsran/scheduler/result/pusch_info.h"

using namespace srsran;
using namespace srs_du;

expected<mac_rx_data_indication>
srs_du::create_test_pdu_with_bsr(du_cell_index_t cell_index, slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id)
{
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Long BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |              L                |  Octet 2
  // | LCG7 | LCG6 |    ...   | LCG0 |  Octet 3
  // |         Buffer Size 1         |  Octet 4

  // We pass BSR=254, which according to TS38.321 is the maximum value for the LBSR size.
  auto buf = byte_buffer::create({0x3e, 0x02, 0x01, 254});
  if (not buf.has_value()) {
    return make_unexpected(default_error_t{});
  }
  return mac_rx_data_indication{
      sl_rx, cell_index, mac_rx_pdu_list{mac_rx_pdu{test_rnti, 0, harq_id, std::move(buf.value())}}};
}

expected<mac_rx_data_indication> srs_du::create_test_pdu_with_rrc_setup_complete(du_cell_index_t cell_index,
                                                                                 slot_point      sl_rx,
                                                                                 rnti_t          test_rnti,
                                                                                 harq_id_t       harq_id)
{
  auto buf = byte_buffer::create({0x01, 0x23, 0xc0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x05, 0xdf, 0x80, 0x10, 0x5e,
                                  0x40, 0x03, 0x40, 0x40, 0x3c, 0x44, 0x3c, 0x3f, 0xc0, 0x00, 0x04, 0x0c, 0x95,
                                  0x1d, 0xa6, 0x0b, 0x80, 0xb8, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00});
  if (not buf.has_value()) {
    return make_unexpected(default_error_t{});
  }
  return mac_rx_data_indication{
      sl_rx, cell_index, mac_rx_pdu_list{mac_rx_pdu{test_rnti, 0, harq_id, std::move(buf.value())}}};
}

static void fill_csi_bits(bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload,
                          rnti_t                                                          rnti,
                          unsigned                                                        nof_ports,
                          unsigned                                                        nof_allowed_ri,
                          const du_test_mode_config::test_mode_ue_config&                 test_ue_cfg)
{
  static constexpr size_t CQI_BITLEN = 4;

  payload.resize(0);
  if (nof_ports == 2) {
    const size_t RI_BITLEN  = std::min(1U, log2_ceil(nof_allowed_ri));
    const size_t PMI_BITLEN = 2;
    payload.push_back(test_ue_cfg.ri - 1, RI_BITLEN);
    payload.push_back(test_ue_cfg.pmi, PMI_BITLEN);
  } else if (nof_ports > 2) {
    const size_t RI_BITLEN    = std::min(2U, log2_ceil(nof_allowed_ri));
    const size_t I_1_1_BITLEN = 3;
    const size_t I_1_3_BITLEN = test_ue_cfg.ri == 2 ? 1 : 0;
    const size_t I_2_BITLEN   = test_ue_cfg.ri == 1 ? 2 : 1;
    payload.push_back(test_ue_cfg.ri - 1, RI_BITLEN);
    if (I_2_BITLEN + I_1_1_BITLEN + I_1_3_BITLEN < 5) {
      payload.push_back(false);
    }
    payload.push_back(test_ue_cfg.i_1_1, I_1_1_BITLEN);
    if (I_1_3_BITLEN > 0) {
      payload.push_back(*test_ue_cfg.i_1_3, I_1_3_BITLEN);
    }
    payload.push_back(test_ue_cfg.i_2, I_2_BITLEN);
  }
  payload.push_back(test_ue_cfg.cqi, CQI_BITLEN);
}

static unsigned get_nof_ports(const csi_report_configuration& csi_rep_cfg)
{
  switch (csi_rep_cfg.pmi_codebook) {
    case pmi_codebook_type::one:
      return 1;
    case pmi_codebook_type::two:
      return 2;
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return 4;
    default:
      report_fatal_error("Unsupported CSI report type");
  }
  return 1;
}

static void fill_csi_bits(bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload,
                          rnti_t                                                          rnti,
                          const pucch_info&                                               pucch,
                          const du_test_mode_config::test_mode_ue_config&                 test_ue_cfg)
{
  unsigned nof_ports      = pucch.csi_rep_cfg.has_value() ? get_nof_ports(pucch.csi_rep_cfg.value()) : 1;
  unsigned nof_allowed_ri = pucch.csi_rep_cfg.has_value() ? pucch.csi_rep_cfg->ri_restriction.count() : nof_ports;
  fill_csi_bits(payload, rnti, nof_ports, nof_allowed_ri, test_ue_cfg);
}

static void fill_csi_bits(bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload,
                          rnti_t                                                          rnti,
                          const ul_sched_info&                                            pusch,
                          const du_test_mode_config::test_mode_ue_config&                 test_ue_cfg)
{
  if (not pusch.uci.has_value() or not pusch.uci.value().csi.has_value()) {
    return;
  }
  const auto& csi_rep_cfg    = pusch.uci.value().csi.value().csi_rep_cfg;
  unsigned    nof_ports      = get_nof_ports(csi_rep_cfg);
  unsigned    nof_allowed_ri = csi_rep_cfg.ri_restriction.count();
  fill_csi_bits(payload, rnti, nof_ports, nof_allowed_ri, test_ue_cfg);
}

static mac_uci_pdu::pucch_f0_or_f1_type make_f0f1_uci_pdu(const pucch_info&                               pucch,
                                                          const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu::pucch_f0_or_f1_type pucch_ind;

  pucch_ind.ul_sinr_dB = 100;
  srsran_assert(pucch.format() == pucch_format::FORMAT_0 or pucch.format() == pucch_format::FORMAT_1,
                "Expected PUCCH Format is F0 or F1");
  if (pucch.format() == pucch_format::FORMAT_0) {
    // In case of Format 0, unlike with Format 0, the GNB only schedules 1 PUCCH per slot; this PUCCH (and the
    // corresponding UCI indication) can have HARQ-ACK bits or SR bits, or both.
    if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
      // In test mode, SRs are never detected, and instead BSR is injected.
      pucch_ind.sr_info.emplace();
      pucch_ind.sr_info.value().detected = false;
    }
    if (pucch.uci_bits.harq_ack_nof_bits > 0) {
      pucch_ind.harq_info.emplace();
      pucch_ind.harq_info->harqs.resize(pucch.uci_bits.harq_ack_nof_bits, uci_pucch_f0_or_f1_harq_values::ack);
    }
  } else {
    if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
      // In test mode, SRs are never detected, and instead BSR is injected.
      pucch_ind.sr_info.emplace();
      pucch_ind.sr_info.value().detected = false;
    }
    if (pucch.uci_bits.harq_ack_nof_bits > 0) {
      pucch_ind.harq_info.emplace();
      // In case of PUCCH F1 with only HARQ-ACK bits, set all HARQ-ACK bits to ACK. If SR is included, then we
      // consider that the PUCCH is not detected.
      auto ack_val = pucch.uci_bits.sr_bits == sr_nof_bits::no_sr ? uci_pucch_f0_or_f1_harq_values::ack
                                                                  : uci_pucch_f0_or_f1_harq_values::dtx;
      pucch_ind.harq_info->harqs.resize(pucch.uci_bits.harq_ack_nof_bits, ack_val);
    }
  }
  return pucch_ind;
}

static mac_uci_pdu::pucch_f2_or_f3_or_f4_type
make_f2f3f4_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  srsran_assert(pucch.format() == pucch_format::FORMAT_2 or pucch.format() == pucch_format::FORMAT_3 or
                    pucch.format() == pucch_format::FORMAT_4,
                "Expected PUCCH Format is F2, F3 or F4");

  const sr_nof_bits sr_bits           = pucch.uci_bits.sr_bits;
  const unsigned    harq_ack_nof_bits = pucch.uci_bits.harq_ack_nof_bits;

  mac_uci_pdu::pucch_f2_or_f3_or_f4_type pucch_ind;
  pucch_ind.ul_sinr_dB = 100;
  if (sr_bits != sr_nof_bits::no_sr) {
    // Set SR to not detected.
    pucch_ind.sr_info.emplace();
    pucch_ind.sr_info->resize(sr_nof_bits_to_uint(sr_bits));
  }
  if (harq_ack_nof_bits > 0) {
    // Set all HARQ-ACK bits to ACK.
    pucch_ind.harq_info.emplace();
    pucch_ind.harq_info->is_valid = true;
    pucch_ind.harq_info->payload.resize(harq_ack_nof_bits);
    pucch_ind.harq_info->payload.fill();
  }
  if (pucch.csi_rep_cfg.has_value()) {
    pucch_ind.csi_part1_info.emplace();
    pucch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(pucch_ind.csi_part1_info->payload, pucch.crnti, pucch, test_ue_cfg);
  }
  return pucch_ind;
}

mac_uci_pdu srs_du::create_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu pdu;
  pdu.rnti = pucch.crnti;
  switch (pucch.format()) {
    case pucch_format::FORMAT_0:
    case pucch_format::FORMAT_1:
      pdu.pdu = make_f0f1_uci_pdu(pucch, test_ue_cfg);
      break;
    case pucch_format::FORMAT_2:
    case pucch_format::FORMAT_3:
    case pucch_format::FORMAT_4:
      pdu.pdu = make_f2f3f4_uci_pdu(pucch, test_ue_cfg);
      break;
    default:
      report_fatal_error("Invalid format");
  }
  return pdu;
}

mac_uci_pdu srs_du::create_uci_pdu(const ul_sched_info&                            pusch,
                                   const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu pdu;
  pdu.rnti                  = pusch.pusch_cfg.rnti;
  auto&           pusch_ind = pdu.pdu.emplace<mac_uci_pdu::pusch_type>();
  const uci_info& uci_info  = *pusch.uci;
  pusch_ind.ul_sinr_dB      = 100;
  if (uci_info.harq.has_value() and uci_info.harq->harq_ack_nof_bits > 0) {
    // If it has HARQ-ACK bits.
    pusch_ind.harq_info.emplace();
    pusch_ind.harq_info->is_valid = true;
    pusch_ind.harq_info->payload.resize(uci_info.harq.value().harq_ack_nof_bits);
    pusch_ind.harq_info->payload.fill();
  }
  if (uci_info.csi.has_value() and uci_info.csi->csi_part1_nof_bits > 0) {
    pusch_ind.csi_part1_info.emplace();
    pusch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(pusch_ind.csi_part1_info->payload, pusch.pusch_cfg.rnti, pusch, test_ue_cfg);
  }
  return pdu;
}

bool srs_du::pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind)
{
  if (pucch.crnti != uci_ind.rnti) {
    return false;
  }
  if ((pucch.format() == pucch_format::FORMAT_0 or pucch.format() == pucch_format::FORMAT_1) and
      std::holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu)) {
    const auto  pucch_pdu_sr_bits = pucch.uci_bits.sr_bits;
    const auto& f0f1_ind          = std::get<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu);
    if (f0f1_ind.sr_info.has_value() != (pucch_pdu_sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    const auto pucch_pdu_harq_bits = pucch.uci_bits.harq_ack_nof_bits;
    if (f0f1_ind.harq_info.has_value() != (pucch_pdu_harq_bits > 0)) {
      return false;
    }
    return true;
  }
  if ((pucch.format() == pucch_format::FORMAT_2 or pucch.format() == pucch_format::FORMAT_3 or
       pucch.format() == pucch_format::FORMAT_4) and
      std::holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu)) {
    const auto& f2f3f4_ind = std::get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu);

    const sr_nof_bits sr_bits           = pucch.uci_bits.sr_bits;
    const unsigned    harq_ack_nof_bits = pucch.uci_bits.harq_ack_nof_bits;

    if (f2f3f4_ind.sr_info.has_value() != (sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    if (f2f3f4_ind.harq_info.has_value() != (harq_ack_nof_bits > 0)) {
      return false;
    }
    if (f2f3f4_ind.csi_part1_info.has_value() != pucch.csi_rep_cfg.has_value()) {
      return false;
    }
    return true;
  }
  return false;
}

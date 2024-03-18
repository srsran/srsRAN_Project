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

#include "mac_test_mode_adapter.h"
#include "du_high_adapter_factories.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/mac/mac_factory.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/scheduler/harq_id.h"
#include <functional>
#include <utility>

using namespace srsran;

// Buffer state that the fake RLC will report to the MAC. This value should be large enough for the scheduler to fill
// the largest TB possible.
static const unsigned TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE = 10000000;

static expected<mac_rx_data_indication> create_test_pdu_with_bsr(slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id)
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
  if (buf.is_error()) {
    return default_error_t{};
  }
  return mac_rx_data_indication{
      sl_rx, to_du_cell_index(0), mac_rx_pdu_list{mac_rx_pdu{test_rnti, 0, harq_id, std::move(buf.value())}}};
}

/// \brief Adapter for the MAC SDU TX builder that auto fills the DL buffer state update.
class test_ue_mac_sdu_tx_builder_adapter : public mac_sdu_tx_builder
{
  static constexpr unsigned TX_SDU_MAX_SIZE = 5000;

public:
  test_ue_mac_sdu_tx_builder_adapter()
  {
    report_fatal_error_if_not(tx_sdu.append(std::vector<uint8_t>(TX_SDU_MAX_SIZE, 0)),
                              "Unable to allocate byte buffer");
  }

  size_t on_new_tx_sdu(span<uint8_t> mac_sdu_buf) override
  {
    // Return empty MAC SDU so that the MAC PDU is padded.
    return 0;
  }

  unsigned on_buffer_state_update() override { return TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE; }

private:
  byte_buffer tx_sdu = {};
};

mac_test_mode_cell_adapter::mac_test_mode_cell_adapter(const srs_du::du_test_config::test_ue_config& test_ue_cfg_,
                                                       const mac_cell_creation_request&              cell_cfg,
                                                       mac_cell_control_information_handler&         adapted_,
                                                       mac_pdu_handler&                              pdu_handler_,
                                                       mac_cell_slot_handler&                        slot_handler_,
                                                       mac_cell_result_notifier&                     result_notifier_,
                                                       std::function<void(rnti_t)>                   dl_bs_notifier_,
                                                       test_ue_info_manager&                         ue_info_mgr_) :
  test_ue_cfg(test_ue_cfg_),
  adapted(adapted_),
  pdu_handler(pdu_handler_),
  slot_handler(slot_handler_),
  result_notifier(result_notifier_),
  dl_bs_notifier(std::move(dl_bs_notifier_)),
  logger(srslog::fetch_basic_logger("MAC")),
  ue_info_mgr(ue_info_mgr_)
{
  // Note: The history ring size has to be a multiple of the TDD frame size in slots.
  const size_t HISTORY_RING_SIZE =
      100 * (cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value()
                 ? nof_slots_per_tdd_period(cell_cfg.sched_req.tdd_ul_dl_cfg_common.value())
                 : 10U);
  sched_decision_history.resize(HISTORY_RING_SIZE);
}

void mac_test_mode_cell_adapter::handle_slot_indication(slot_point sl_tx)
{
  if (test_ue_cfg.auto_ack_indication_delay.has_value()) {
    // auto-generation of CRC/UCI indication is enabled.
    slot_point                    sl_rx = sl_tx - test_ue_cfg.auto_ack_indication_delay.value();
    const slot_descision_history& entry = sched_decision_history[sl_rx.to_uint() % sched_decision_history.size()];

    // Handle auto-generation of pending CRC indications.
    if (not entry.puschs.empty()) {
      mac_crc_indication_message crc_ind{};
      crc_ind.sl_rx = sl_rx;

      for (const ul_sched_info& pusch : entry.puschs) {
        auto& crc_pdu   = crc_ind.crcs.emplace_back();
        crc_pdu.rnti    = pusch.pusch_cfg.rnti;
        crc_pdu.harq_id = pusch.pusch_cfg.harq_id;
        // Force CRC=OK for test UE.
        crc_pdu.tb_crc_success = true;
        // Force UL SINR.
        crc_pdu.ul_sinr_metric = 100;
      }

      // Forward CRC to the real MAC.
      forward_crc_ind_to_mac(crc_ind);
    }

    // Handle auto-generation of pending UCI indications.
    mac_uci_indication_message uci_ind;
    uci_ind.sl_rx = sl_rx;

    // > Handle pending PUCCHs.
    for (const pucch_info& pucch : entry.pucchs) {
      mac_uci_pdu& pdu = uci_ind.ucis.emplace_back();
      pdu.rnti         = pucch.crnti;
      switch (pucch.format) {
        case pucch_format::FORMAT_1: {
          fill_uci_pdu(pdu.pdu.emplace<mac_uci_pdu::pucch_f0_or_f1_type>(), pucch);
        } break;
        case pucch_format::FORMAT_2: {
          fill_uci_pdu(pdu.pdu.emplace<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(), pucch);
        } break;
        default:
          break;
      }
    }

    // > Handle pending PUSCHs.
    for (const ul_sched_info& pusch : entry.puschs) {
      if (pusch.uci.has_value()) {
        mac_uci_pdu& pdu = uci_ind.ucis.emplace_back();
        pdu.rnti         = pusch.pusch_cfg.rnti;
        fill_uci_pdu(pdu.pdu.emplace<mac_uci_pdu::pusch_type>(), pusch);
      }
    }

    // Forward UCI indication to real MAC.
    forward_uci_ind_to_mac(uci_ind);
  }

  slot_handler.handle_slot_indication(sl_tx);
}

void mac_test_mode_cell_adapter::handle_error_indication(slot_point sl_tx, error_event event)
{
  slot_descision_history& entry = sched_decision_history[sl_tx.to_uint() % sched_decision_history.size()];

  if (event.pusch_and_pucch_discarded) {
    // Delete expected UCI and CRC indications that resulted from the scheduler decisions for this slot.
    entry.puschs.clear();
    entry.pucchs.clear();
  }

  slot_handler.handle_error_indication(sl_tx, event);
}

void mac_test_mode_cell_adapter::handle_crc(const mac_crc_indication_message& msg)
{
  // Forward CRC to MAC, but remove the UCI for the test mode UE.
  mac_crc_indication_message msg_copy;
  msg_copy.sl_rx = msg.sl_rx;
  for (const mac_crc_pdu& crc : msg.crcs) {
    if (ue_info_mgr.is_test_ue(crc.rnti)) {
      // test mode UE case.

      // Find respective PUSCH PDU that was previously scheduled.
      const slot_descision_history& entry = sched_decision_history[msg.sl_rx.to_uint() % sched_decision_history.size()];
      bool found = std::any_of(entry.puschs.begin(), entry.puschs.end(), [&](const ul_sched_info& pusch) {
        return pusch.pusch_cfg.rnti == crc.rnti and pusch.pusch_cfg.harq_id == crc.harq_id;
      });
      if (not found) {
        logger.warning(
            "c-rnti={}: Mismatch between provided CRC and expected PUSCH for slot_rx={}", crc.rnti, msg.sl_rx);
        continue;
      }

      // Intercept the CRC indication and force crc=OK and UL SNR.
      mac_crc_pdu test_crc    = crc;
      test_crc.tb_crc_success = true;
      test_crc.ul_sinr_metric = 100;
      msg_copy.crcs.push_back(test_crc);

    } else {
      // non-test mode UE. Forward the original CRC PDU.
      msg_copy.crcs.push_back(crc);
    }
  }

  // Forward resulting CRC indication to real MAC.
  forward_crc_ind_to_mac(msg_copy);
}

void mac_test_mode_cell_adapter::fill_uci_pdu(mac_uci_pdu::pucch_f0_or_f1_type& pucch_ind,
                                              const pucch_info&                 pucch) const
{
  pucch_ind.ul_sinr = 100;
  if (pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
    // In test mode, SRs are never detected, and instead BSR is injected.
    pucch_ind.sr_info.emplace();
    pucch_ind.sr_info.value().detected = false;
  }
  if (pucch.format_1.harq_ack_nof_bits > 0) {
    pucch_ind.harq_info.emplace();
    // In case of PUCCH F1 with only HARQ-ACK bits, set all HARQ-ACK bits to ACK. If SR is included, then we
    // consider that the PUCCH is not detected.
    auto ack_val = pucch.format_1.sr_bits == sr_nof_bits::no_sr ? uci_pucch_f0_or_f1_harq_values::ack
                                                                : uci_pucch_f0_or_f1_harq_values::dtx;
    pucch_ind.harq_info->harqs.resize(pucch.format_1.harq_ack_nof_bits, ack_val);
  }
}

void mac_test_mode_cell_adapter::fill_uci_pdu(mac_uci_pdu::pucch_f2_or_f3_or_f4_type& pucch_ind,
                                              const pucch_info&                       pucch) const
{
  pucch_ind.ul_sinr = 100;
  if (pucch.format_2.sr_bits != sr_nof_bits::no_sr) {
    // Set SR to not detected.
    pucch_ind.sr_info.emplace();
    pucch_ind.sr_info->resize(sr_nof_bits_to_uint(pucch.format_2.sr_bits));
  }
  if (pucch.format_2.harq_ack_nof_bits > 0) {
    // Set all HARQ-ACK bits to ACK.
    pucch_ind.harq_info.emplace();
    pucch_ind.harq_info->is_valid = true;
    pucch_ind.harq_info->payload.resize(pucch.format_2.harq_ack_nof_bits);
    pucch_ind.harq_info->payload.fill();
  }
  if (pucch.csi_rep_cfg.has_value()) {
    pucch_ind.csi_part1_info.emplace();
    pucch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(pucch.crnti, pucch_ind.csi_part1_info->payload);
  }
}

void mac_test_mode_cell_adapter::fill_uci_pdu(mac_uci_pdu::pusch_type& pusch_ind, const ul_sched_info& ul_grant) const
{
  const uci_info& uci_info = *ul_grant.uci;
  pusch_ind.ul_sinr        = 100;
  if (uci_info.harq.has_value() and uci_info.harq->harq_ack_nof_bits > 0) {
    pusch_ind.harq_info.emplace();
    pusch_ind.harq_info->is_valid = true;
    pusch_ind.harq_info->payload.resize(uci_info.harq.value().harq_ack_nof_bits);
    pusch_ind.harq_info->payload.fill();
  }
  if (uci_info.csi.has_value() and uci_info.csi->csi_part1_nof_bits > 0) {
    pusch_ind.csi_part1_info.emplace();
    pusch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(ul_grant.pusch_cfg.rnti, pusch_ind.csi_part1_info->payload);
  }
}

static bool pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind)
{
  if (pucch.crnti != uci_ind.rnti) {
    return false;
  }
  if (pucch.format == pucch_format::FORMAT_1 and
      variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu)) {
    const auto& f1_ind = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu);
    if (f1_ind.sr_info.has_value() != (pucch.format_1.sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    if (f1_ind.harq_info.has_value() != (pucch.format_1.harq_ack_nof_bits > 0)) {
      return false;
    }
    return true;
  }
  if (pucch.format == pucch_format::FORMAT_2 and
      variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu)) {
    const auto& f2_ind = variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu);
    if (f2_ind.sr_info.has_value() != (pucch.format_2.sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    if (f2_ind.harq_info.has_value() != (pucch.format_2.harq_ack_nof_bits > 0)) {
      return false;
    }
    if (f2_ind.csi_part1_info.has_value() != pucch.csi_rep_cfg.has_value()) {
      return false;
    }
    return true;
  }
  return false;
}

void mac_test_mode_cell_adapter::forward_uci_ind_to_mac(const mac_uci_indication_message& uci_msg)
{
  if (uci_msg.ucis.empty()) {
    return;
  }

  // Forward UCI indication to real MAC.
  adapted.handle_uci(uci_msg);

  // Update buffer states.
  for (const mac_uci_pdu& pdu : uci_msg.ucis) {
    if (ue_info_mgr.is_test_ue(pdu.rnti) and variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(pdu.pdu)) {
      const auto& f1_ind = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(pdu.pdu);

      if (f1_ind.harq_info.has_value()) {
        // In case of PUCCH F1 with HARQ-ACK bits, we assume that the Msg4 has been received. At this point, we
        // update the test UE with positive DL buffer states and BSR.
        if (not ue_info_mgr.is_msg4_rxed(pdu.rnti)) {
          if (test_ue_cfg.pdsch_active) {
            // Update DL buffer state automatically.
            dl_bs_notifier(pdu.rnti);
          }

          if (test_ue_cfg.pusch_active) {
            auto rx_pdu = create_test_pdu_with_bsr(uci_msg.sl_rx, pdu.rnti, to_harq_id(0));
            if (rx_pdu.is_error()) {
              logger.warning("Unable to create test PDU with BSR");
              continue;
            }
            // In case of PUSCH test mode is enabled, push a BSR to trigger the first PUSCH.
            pdu_handler.handle_rx_data_indication(std::move(rx_pdu.value()));
          }
          ue_info_mgr.msg4_rxed(pdu.rnti, true);
        }
      }
    }
  }
}

void mac_test_mode_cell_adapter::forward_crc_ind_to_mac(const mac_crc_indication_message& crc_msg)
{
  if (crc_msg.crcs.empty()) {
    return;
  }

  // Forward CRC indication to real MAC.
  adapted.handle_crc(crc_msg);

  if (not test_ue_cfg.pusch_active) {
    return;
  }

  for (const mac_crc_pdu& pdu : crc_msg.crcs) {
    if (not ue_info_mgr.is_test_ue(pdu.rnti)) {
      continue;
    }

    auto rx_pdu = create_test_pdu_with_bsr(crc_msg.sl_rx, pdu.rnti, to_harq_id(pdu.harq_id));
    if (rx_pdu.is_error()) {
      logger.warning("Unable to create test PDU with BSR");
      continue;
    }
    // In case of test mode UE, auto-forward a positive BSR.
    pdu_handler.handle_rx_data_indication(std::move(rx_pdu.value()));
  }
}

void mac_test_mode_cell_adapter::handle_uci(const mac_uci_indication_message& msg)
{
  const slot_descision_history& entry = sched_decision_history[msg.sl_rx.to_uint() % sched_decision_history.size()];

  // Forward UCI to MAC, but alter the UCI for the test mode UE.
  mac_uci_indication_message msg_copy;
  msg_copy.sl_rx = msg.sl_rx;
  for (const mac_uci_pdu& pdu : msg.ucis) {
    if (not ue_info_mgr.is_test_ue(pdu.rnti)) {
      // non-test mode UE. Forward the original UCI indication PDU.
      msg_copy.ucis.push_back(pdu);
    } else {
      // test mode UE.
      // Intercept the UCI indication and force HARQ-ACK=ACK and CSI.
      msg_copy.ucis.push_back(pdu);
      mac_uci_pdu& test_uci = msg_copy.ucis.back();

      bool entry_found = false;
      if (variant_holds_alternative<mac_uci_pdu::pusch_type>(test_uci.pdu)) {
        for (const ul_sched_info& pusch : entry.puschs) {
          if (pusch.pusch_cfg.rnti == pdu.rnti and pusch.uci.has_value()) {
            fill_uci_pdu(variant_get<mac_uci_pdu::pusch_type>(test_uci.pdu), pusch);
            entry_found = true;
          }
        }
      } else {
        // PUCCH case.
        for (const pucch_info& pucch : entry.pucchs) {
          if (pucch_info_and_uci_ind_match(pucch, test_uci)) {
            // Intercept the UCI indication and force HARQ-ACK=ACK and UCI.
            if (pucch.format == pucch_format::FORMAT_1) {
              fill_uci_pdu(variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(test_uci.pdu), pucch);
            } else {
              fill_uci_pdu(variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(test_uci.pdu), pucch);
            }
            entry_found = true;
          }
        }
      }

      if (not entry_found) {
        msg_copy.ucis.pop_back();
        logger.warning("c-rnti={}: Mismatch between provided UCI and expected UCI for slot_rx={}", pdu.rnti, msg.sl_rx);
      }
    }
  }

  // Forward UCI indication to real MAC.
  forward_uci_ind_to_mac(msg_copy);
}

// Intercepts the UL results coming from the MAC.
void mac_test_mode_cell_adapter::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  slot_descision_history& entry = sched_decision_history[ul_res.slot.to_uint() % sched_decision_history.size()];

  // Resets the history for this ring element.
  entry.pucchs.clear();
  entry.puschs.clear();

  // Fill the ring element with the scheduler decisions.
  for (const pucch_info& pucch : ul_res.ul_res->pucchs) {
    if (ue_info_mgr.is_test_ue(pucch.crnti)) {
      entry.pucchs.push_back(pucch);
    }
  }
  for (const ul_sched_info& pusch : ul_res.ul_res->puschs) {
    if (ue_info_mgr.is_test_ue(pusch.pusch_cfg.rnti)) {
      entry.puschs.push_back(pusch);
    }
  }

  result_notifier.on_new_uplink_scheduler_results(ul_res);
}

void mac_test_mode_cell_adapter::fill_csi_bits(
    rnti_t                                                          rnti,
    bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload) const
{
  static constexpr size_t CQI_BITLEN = 4;

  const sched_ue_config_request& ue_cfg_req = ue_info_mgr.get_sched_ue_cfg_request(rnti);

  if (ue_cfg_req.cells->empty() or not(*ue_cfg_req.cells)[0].serv_cell_cfg.csi_meas_cfg.has_value()) {
    return;
  }
  payload.resize(0);

  unsigned nof_ports = (*ue_cfg_req.cells)[0].serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list[0].res_mapping.nof_ports;
  if (nof_ports == 2) {
    const size_t RI_BITLEN  = 1;
    const size_t PMI_BITLEN = 2;
    payload.push_back(test_ue_cfg.ri - 1, RI_BITLEN);
    payload.push_back(test_ue_cfg.pmi, PMI_BITLEN);
  } else if (nof_ports > 2) {
    const size_t RI_BITLEN    = 2;
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

// ----

mac_cell_result_notifier& phy_test_mode_adapter::get_cell(du_cell_index_t cell_index)
{
  return cells[cell_index];
}
void phy_test_mode_adapter::connect(du_cell_index_t cell_index, mac_cell_result_notifier& test_mode_cell_notifier)
{
  cells[cell_index].ptr = &test_mode_cell_notifier;
}
void phy_test_mode_adapter::disconnect(du_cell_index_t cell_index)
{
  cells[cell_index].ptr = nullptr;
}
void phy_test_mode_adapter::phy_cell::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  ptr->on_new_downlink_scheduler_results(dl_res);
}
void phy_test_mode_adapter::phy_cell::on_new_downlink_data(const mac_dl_data_result& dl_data)
{
  ptr->on_new_downlink_data(dl_data);
}
void phy_test_mode_adapter::phy_cell::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  ptr->on_new_uplink_scheduler_results(ul_res);
}
void phy_test_mode_adapter::phy_cell::on_cell_results_completion(slot_point slot)
{
  ptr->on_cell_results_completion(slot);
}

// ----

mac_test_mode_adapter::mac_test_mode_adapter(const srs_du::du_test_config::test_ue_config& test_ue_cfg_,
                                             mac_result_notifier&                          phy_notifier_) :
  test_ue(test_ue_cfg_),
  phy_notifier(std::make_unique<phy_test_mode_adapter>(phy_notifier_)),
  ue_info_mgr(test_ue.rnti, test_ue.nof_ues)
{
}

mac_test_mode_adapter::~mac_test_mode_adapter() = default;

void mac_test_mode_adapter::connect(std::unique_ptr<mac_interface> mac_ptr)
{
  mac_adapted = std::move(mac_ptr);
}

void mac_test_mode_adapter::add_cell(const mac_cell_creation_request& cell_cfg)
{
  // Create cell in real MAC.
  mac_adapted->get_cell_manager().add_cell(cell_cfg);

  // Create the cell in the MAC test mode.
  auto func_dl_bs_push = [this](rnti_t rnti) {
    get_ue_control_info_handler().handle_dl_buffer_state_update(
        {ue_info_mgr.rnti_to_du_ue_idx(rnti), lcid_t::LCID_SRB1, TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE});
  };
  auto new_cell =
      std::make_unique<mac_test_mode_cell_adapter>(test_ue,
                                                   cell_cfg,
                                                   mac_adapted->get_control_info_handler(cell_cfg.cell_index),
                                                   mac_adapted->get_pdu_handler(),
                                                   mac_adapted->get_slot_handler(cell_cfg.cell_index),
                                                   phy_notifier->adapted_phy.get_cell(cell_cfg.cell_index),
                                                   func_dl_bs_push,
                                                   ue_info_mgr);

  if (cell_info_handler.size() <= cell_cfg.cell_index) {
    cell_info_handler.resize(cell_cfg.cell_index + 1);
  }
  cell_info_handler[cell_cfg.cell_index] = std::move(new_cell);

  phy_notifier->connect(cell_cfg.cell_index, *cell_info_handler[cell_cfg.cell_index]);
}

void mac_test_mode_adapter::remove_cell(du_cell_index_t cell_index)
{
  phy_notifier->disconnect(cell_index);

  // Remove cell in the MAC test mode.
  cell_info_handler[cell_index].reset();

  // Remove cell in real MAC.
  mac_adapted->get_cell_manager().remove_cell(cell_index);
}

mac_cell_controller& mac_test_mode_adapter::get_cell_controller(du_cell_index_t cell_index)
{
  return mac_adapted->get_cell_manager().get_cell_controller(cell_index);
}

mac_cell_control_information_handler& mac_test_mode_adapter::get_control_info_handler(du_cell_index_t cell_index)
{
  return *cell_info_handler[cell_index];
}

void mac_test_mode_adapter::handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs)
{
  mac_dl_buffer_state_indication_message dl_bs_copy = dl_bs;
  if (ue_info_mgr.is_test_ue(dl_bs.ue_index) and test_ue.pdsch_active and dl_bs.lcid != LCID_SRB0) {
    // It is the test UE. Set a positive DL buffer state if PDSCH is set to "activated".
    dl_bs_copy.bs = TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE;
  }
  mac_adapted->get_ue_control_info_handler().handle_dl_buffer_state_update(dl_bs_copy);
}

std::vector<mac_logical_channel_config>
mac_test_mode_adapter::adapt_bearers(const std::vector<mac_logical_channel_config>& orig_bearers)
{
  static test_ue_mac_sdu_tx_builder_adapter dl_adapter;

  auto test_ue_adapted_bearers = orig_bearers;
  for (auto& bearer : test_ue_adapted_bearers) {
    if (bearer.lcid != lcid_t::LCID_SRB0) {
      if (test_ue.pdsch_active) {
        bearer.dl_bearer = &dl_adapter;
      }
    }
  }

  return test_ue_adapted_bearers;
}

async_task<mac_ue_create_response> mac_test_mode_adapter::handle_ue_create_request(const mac_ue_create_request& cfg)
{
  if (ue_info_mgr.is_test_ue(cfg.crnti)) {
    // It is the test UE.
    mac_ue_create_request cfg_copy = cfg;

    // Save UE index and configuration of test mode UE.
    ue_info_mgr.add_ue(cfg.crnti, cfg_copy.ue_index, cfg_copy.sched_cfg);

    // Add adapters to the UE config bearers before passing it to MAC.
    cfg_copy.bearers = adapt_bearers(cfg.bearers);

    // Forward test UE creation request to MAC.
    return mac_adapted->get_ue_configurator().handle_ue_create_request(cfg_copy);
  }

  // Forward normal UE creation request to MAC.
  return mac_adapted->get_ue_configurator().handle_ue_create_request(cfg);
}

async_task<mac_ue_reconfiguration_response>
mac_test_mode_adapter::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg)
{
  if (ue_info_mgr.is_test_ue(cfg.crnti)) {
    // If it is the test UE.
    auto cfg_adapted = cfg;

    // Add adapters to the UE config bearers before passing it to MAC.
    cfg_adapted.bearers_to_addmod = adapt_bearers(cfg.bearers_to_addmod);

    return mac_adapted->get_ue_configurator().handle_ue_reconfiguration_request(cfg_adapted);
  }

  // otherwise, just forward config.
  return mac_adapted->get_ue_configurator().handle_ue_reconfiguration_request(cfg);
}

async_task<mac_ue_delete_response> mac_test_mode_adapter::handle_ue_delete_request(const mac_ue_delete_request& cfg)
{
  return mac_adapted->get_ue_configurator().handle_ue_delete_request(cfg);
}

bool mac_test_mode_adapter::handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)
{
  return mac_adapted->get_ue_configurator().handle_ul_ccch_msg(ue_index, std::move(pdu));
}

std::unique_ptr<mac_interface> srsran::create_du_high_mac(const mac_config&             mac_cfg,
                                                          const srs_du::du_test_config& test_cfg)
{
  if (not test_cfg.test_ue.has_value()) {
    return create_mac(mac_cfg);
  }

  // Create a MAC test mode adapter that wraps the real MAC.
  auto mac_testmode = std::make_unique<mac_test_mode_adapter>(*test_cfg.test_ue, mac_cfg.phy_notifier);
  mac_testmode->connect(create_mac(mac_config{mac_cfg.ul_ccch_notifier,
                                              mac_cfg.ue_exec_mapper,
                                              mac_cfg.cell_exec_mapper,
                                              mac_cfg.ctrl_exec,
                                              mac_testmode->get_phy_notifier(),
                                              mac_cfg.mac_cfg,
                                              mac_cfg.pcap,
                                              mac_cfg.sched_cfg,
                                              mac_cfg.metric_notifier}));
  return mac_testmode;
}

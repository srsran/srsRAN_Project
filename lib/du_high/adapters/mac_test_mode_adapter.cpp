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

using namespace srsran;

// Buffer state that the fake RLC will report to the MAC. This value should be large enough for the scheduler to fill
// the largest TB possible.
static const unsigned TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE = 10000000;

static mac_rx_data_indication create_test_pdu_with_bsr(slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id)
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
  return mac_rx_data_indication{sl_rx,
                                to_du_cell_index(0),
                                mac_rx_pdu_list{mac_rx_pdu{test_rnti, 0, harq_id, byte_buffer{0x3e, 0x02, 0x01, 254}}}};
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
                                                       std::function<void()>                         dl_bs_notifier_,
                                                       const sched_ue_config_request&                ue_cfg_req_) :
  test_ue_cfg(test_ue_cfg_),
  adapted(adapted_),
  pdu_handler(pdu_handler_),
  slot_handler(slot_handler_),
  result_notifier(result_notifier_),
  dl_bs_notifier(dl_bs_notifier_),
  ue_cfg_req(ue_cfg_req_)
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
  slot_point              sl_rx = sl_tx - MAX_UL_DL_DELAY;
  slot_descision_history& entry = sched_decision_history[sl_rx.to_uint() % sched_decision_history.size()];

  if (not entry.pucchs.empty()) {
    mac_uci_indication_message uci_ind;
    uci_ind.sl_rx = sl_rx;
    for (const pucch_info& pucch : entry.pucchs) {
      mac_uci_pdu& pdu = uci_ind.ucis.emplace_back();
      pdu.rnti         = pucch.crnti;
      switch (pucch.format) {
        case pucch_format::FORMAT_1: {
          auto& f1   = pdu.pdu.emplace<mac_uci_pdu::pucch_f0_or_f1_type>();
          f1.ul_sinr = 100;
          if (pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
            f1.sr_info.emplace();
            f1.sr_info.value().sr_detected = false;
          }
          if (pucch.format_1.harq_ack_nof_bits > 0) {
            f1.harq_info.emplace();
            // In case of PUCCH F1 with only HARQ-ACK bits, set all HARQ-ACK bits to ACK. If SR is included, then we
            // consider that the PUCCH is not detected.
            auto ack_val = pucch.format_1.sr_bits == sr_nof_bits::no_sr ? uci_pucch_f0_or_f1_harq_values::ack
                                                                        : uci_pucch_f0_or_f1_harq_values::dtx;
            f1.harq_info->harqs.resize(pucch.format_1.harq_ack_nof_bits, ack_val);

            // In case of PUCCH F1 with HARQ-ACK bits, we assume that the Msg4 has been received. At this point, we
            // update the test UE with positive DL buffer states and BSR.
            if (not msg4_rx_flag) {
              if (test_ue_cfg.pdsch_active) {
                // Update DL buffer state automatically.
                dl_bs_notifier();
              }

              if (test_ue_cfg.pusch_active) {
                // In case of PUSCH test mode is enabled, push a BSR.
                pdu_handler.handle_rx_data_indication(create_test_pdu_with_bsr(sl_rx, test_ue_cfg.rnti, to_harq_id(0)));
              }
              msg4_rx_flag = true;
            }
          }
        } break;
        case pucch_format::FORMAT_2: {
          auto& f2   = pdu.pdu.emplace<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>();
          f2.ul_sinr = 100;
          if (pucch.format_2.harq_ack_nof_bits > 0) {
            f2.harq_info.emplace();
            f2.harq_info->harq_status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
            f2.harq_info->payload.resize(pucch.format_2.harq_ack_nof_bits);
            f2.harq_info->payload.fill();
          }
          if (pucch.format_2.sr_bits != sr_nof_bits::no_sr) {
            f2.sr_info.emplace();
            f2.sr_info->resize(sr_nof_bits_to_uint(pucch.format_2.sr_bits));
          }
          if (pucch.csi_rep_cfg.has_value()) {
            f2.uci_part1_or_csi_part1_info.emplace();
            f2.uci_part1_or_csi_part1_info->payload_type = mac_uci_pdu::pucch_f2_or_f3_or_f4_type::
                uci_payload_or_csi_information::payload_type_t::csi_part_payload;
            f2.uci_part1_or_csi_part1_info->status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
            fill_csi_bits(f2.uci_part1_or_csi_part1_info->payload);
          }
        } break;
        default:
          break;
      }
    }

    // Forward MAC UCI to the real MAC.
    adapted.handle_uci(uci_ind);
  }

  if (entry.pusch.has_value()) {
    // In case there is UCI in the PUSCH, send an UCI indication to real MAC.
    if (entry.pusch->uci.has_value()) {
      // Prepare the PUSCH UCI.
      mac_uci_indication_message uci_ind;
      uci_ind.sl_rx     = sl_rx;
      mac_uci_pdu& pdu  = uci_ind.ucis.emplace_back();
      pdu.rnti          = test_ue_cfg.rnti;
      auto& pusch_uci   = pdu.pdu.emplace<mac_uci_pdu::pusch_type>();
      pusch_uci.ul_sinr = 100;
      if (entry.pusch->uci.value().harq.has_value() and entry.pusch->uci.value().harq.value().harq_ack_nof_bits > 0) {
        pusch_uci.harq_info.emplace();
        pusch_uci.harq_info->harq_status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
        pusch_uci.harq_info->payload.resize(entry.pusch->uci.value().harq.value().harq_ack_nof_bits);
        pusch_uci.harq_info->payload.fill();
      }
      if (entry.pusch->uci.value().csi.has_value() and entry.pusch->uci.value().csi.value().csi_part1_nof_bits > 0) {
        pusch_uci.csi_part1_info.emplace();
        pusch_uci.csi_part1_info->csi_status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
        fill_csi_bits(pusch_uci.csi_part1_info->payload);
        unsigned nof_ports =
            (*ue_cfg_req.cells)[0].serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list[0].res_mapping.nof_ports;
        if (nof_ports > 1) {
          pusch_uci.ri  = test_ue_cfg.ri;
          pusch_uci.pmi = test_ue_cfg.pmi;
        }
      }

      // Forward MAC UCI to the real MAC.
      adapted.handle_uci(uci_ind);
    }

    // Handle pending CRC indications
    mac_crc_indication_message crc_ind{};
    crc_ind.sl_rx   = sl_rx;
    auto& crc_pdu   = crc_ind.crcs.emplace_back();
    crc_pdu.rnti    = test_ue_cfg.rnti;
    crc_pdu.harq_id = entry.pusch->pusch_cfg.harq_id;
    // Force CRC=OK for test UE.
    crc_pdu.tb_crc_success = true;
    // Force UL SINR.
    crc_pdu.ul_sinr_metric = 100;

    // Forward CRC to the real MAC.
    adapted.handle_crc(crc_ind);

    if (test_ue_cfg.pusch_active) {
      // In case of PUSCH test mode is enabled, push a BSR.
      pdu_handler.handle_rx_data_indication(
          create_test_pdu_with_bsr(sl_rx, test_ue_cfg.rnti, to_harq_id(crc_pdu.harq_id)));
    }
  }

  slot_handler.handle_slot_indication(sl_tx);
}

void mac_test_mode_cell_adapter::handle_error_indication(slot_point sl_tx, error_event event)
{
  slot_descision_history& entry = sched_decision_history[sl_tx.to_uint() % sched_decision_history.size()];

  if (event.pusch_and_pucch_discarded) {
    // Delete expected UCI and CRC indications that resulted from the scheduler decisions for this slot.
    entry.pusch.reset();
    entry.pucchs.clear();
  }

  slot_handler.handle_error_indication(sl_tx, event);
}

void mac_test_mode_cell_adapter::handle_crc(const mac_crc_indication_message& msg)
{
  auto pdu_it = std::find_if(
      msg.crcs.begin(), msg.crcs.end(), [this](const mac_crc_pdu& pdu) { return pdu.rnti == test_ue_cfg.rnti; });
  if (pdu_it == msg.crcs.end()) {
    // No CRC for the test mode UE. Just forward message to MAC.
    adapted.handle_crc(msg);
    return;
  }

  // Forward CRC to MAC, but remove the UCI for the test mode UE.
  mac_crc_indication_message msg_copy;
  msg_copy.sl_rx = msg.sl_rx;
  for (auto& crc : msg.crcs) {
    if (crc.rnti != test_ue_cfg.rnti) {
      msg_copy.crcs.push_back(crc);
    }
  }
  if (not msg_copy.crcs.empty()) {
    adapted.handle_crc(msg_copy);
  }
}

void mac_test_mode_cell_adapter::handle_uci(const mac_uci_indication_message& msg)
{
  auto uci_it = std::find_if(
      msg.ucis.begin(), msg.ucis.end(), [this](const mac_uci_pdu& uci) { return uci.rnti == test_ue_cfg.rnti; });
  if (uci_it == msg.ucis.end()) {
    // No UCI for the test mode UE. Just forward message to MAC.
    adapted.handle_uci(msg);
    return;
  }

  // Forward UCI to MAC, but remove the UCI for the test mode UE.
  mac_uci_indication_message msg_copy;
  msg_copy.sl_rx = msg.sl_rx;
  for (const mac_uci_pdu& pdu : msg.ucis) {
    if (pdu.rnti != test_ue_cfg.rnti) {
      msg_copy.ucis.push_back(pdu);
    }
  }
  if (not msg_copy.ucis.empty()) {
    adapted.handle_uci(msg_copy);
  }
}

// Intercepts the UL results coming from the MAC.
void mac_test_mode_cell_adapter::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  slot_descision_history& entry = sched_decision_history[ul_res.slot.to_uint() % sched_decision_history.size()];

  // Resets the history for this ring element.
  entry.pucchs.clear();
  entry.pusch.reset();

  // Fill the ring element with the scheduler decisions.
  for (const pucch_info& pucch : ul_res.ul_res->pucchs) {
    if (pucch.crnti == test_ue_cfg.rnti) {
      entry.pucchs.push_back(pucch);
    }
  }
  for (const ul_sched_info& pusch : ul_res.ul_res->puschs) {
    if (pusch.pusch_cfg.rnti == test_ue_cfg.rnti) {
      entry.pusch = pusch;
    }
  }

  result_notifier.on_new_uplink_scheduler_results(ul_res);
}

void mac_test_mode_cell_adapter::fill_csi_bits(bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload)
{
  static constexpr size_t CQI_BITLEN = 4;

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
  test_ue(test_ue_cfg_), phy_notifier(std::make_unique<phy_test_mode_adapter>(phy_notifier_))
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
  auto func_dl_bs_push = [this]() {
    get_ue_control_info_handler().handle_dl_buffer_state_update(
        {test_ue_index, lcid_t::LCID_SRB1, TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE});
  };
  auto new_cell =
      std::make_unique<mac_test_mode_cell_adapter>(test_ue,
                                                   cell_cfg,
                                                   mac_adapted->get_control_info_handler(cell_cfg.cell_index),
                                                   mac_adapted->get_pdu_handler(),
                                                   mac_adapted->get_slot_handler(cell_cfg.cell_index),
                                                   phy_notifier->adapted_phy.get_cell(cell_cfg.cell_index),
                                                   func_dl_bs_push,
                                                   test_ue_cfg);

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
  if (test_ue_index == dl_bs.ue_index and test_ue.pdsch_active and dl_bs.lcid != LCID_SRB0) {
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
  if (cfg.crnti == test_ue.rnti) {
    // It is the test UE.
    mac_ue_create_request cfg_copy = cfg;

    // Save UE index.
    test_ue_index = cfg_copy.ue_index;

    // Add adapters to the UE config bearers before passing it to MAC.
    cfg_copy.bearers = adapt_bearers(cfg.bearers);

    // Save config of test mode UE.
    test_ue_cfg = cfg_copy.sched_cfg;

    // Forward test UE creation request to MAC.
    return mac_adapted->get_ue_configurator().handle_ue_create_request(cfg_copy);
  }

  // Forward normal UE creation request to MAC.
  return mac_adapted->get_ue_configurator().handle_ue_create_request(cfg);
}

async_task<mac_ue_reconfiguration_response>
mac_test_mode_adapter::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg)
{
  if (cfg.crnti == test_ue.rnti) {
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

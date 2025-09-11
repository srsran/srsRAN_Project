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

#include "mac_test_mode_adapter.h"
#include "../adapters/du_high_adapter_factories.h"
#include "mac_test_mode_helpers.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/mac/mac_factory.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/result/sched_result.h"
#include <functional>
#include <utility>

using namespace srsran;
using namespace srs_du;

// Buffer state that the fake RLC will report to the MAC. This value should be large enough for the scheduler to fill
// the largest TB possible.
static const unsigned TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE = 10000000;

namespace {

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

  rlc_buffer_state on_buffer_state_update() override
  {
    rlc_buffer_state bs = {};
    bs.pending_bytes    = TEST_UE_DL_BUFFER_STATE_UPDATE_SIZE;
    // TODO: set bs.hol_toa
    return bs;
  }

private:
  byte_buffer tx_sdu;
};

} // namespace

mac_test_mode_cell_adapter::mac_test_mode_cell_adapter(
    const srs_du::du_test_mode_config::test_mode_ue_config& test_ue_cfg_,
    const mac_cell_creation_request&                        cell_cfg,
    mac_cell_control_information_handler&                   adapted_,
    mac_pdu_handler&                                        pdu_handler_,
    mac_cell_slot_handler&                                  slot_handler_,
    mac_cell_result_notifier&                               result_notifier_,
    std::function<void(rnti_t)>                             dl_bs_notifier_,
    mac_test_mode_event_handler&                            event_handler_,
    mac_test_mode_ue_repository&                            ue_info_mgr_) :
  cell_index(cell_cfg.cell_index),
  test_ue_cfg(test_ue_cfg_),
  adapted(adapted_),
  pdu_handler(pdu_handler_),
  slot_handler(slot_handler_),
  result_notifier(result_notifier_),
  dl_bs_notifier(std::move(dl_bs_notifier_)),
  logger(srslog::fetch_basic_logger("MAC")),
  history(cell_cfg),
  event_handler(event_handler_),
  ue_info_mgr(ue_info_mgr_)
{
}

void mac_test_mode_cell_adapter::handle_slot_indication(const mac_cell_timing_context& context)
{
  if (test_ue_cfg.auto_ack_indication_delay.has_value()) {
    // auto-generation of CRC/UCI indication is enabled.
    slot_point sl_rx = context.sl_tx - test_ue_cfg.auto_ack_indication_delay.value();
    const auto entry = history.read(sl_rx);
    if (entry != nullptr) {
      // Handle auto-generation of pending CRC indications.
      if (not entry->puschs.empty()) {
        mac_crc_indication_message crc_ind{};
        crc_ind.sl_rx = sl_rx;

        for (const ul_sched_info& pusch : entry->puschs) {
          auto& crc_pdu   = crc_ind.crcs.emplace_back();
          crc_pdu.rnti    = pusch.pusch_cfg.rnti;
          crc_pdu.harq_id = pusch.pusch_cfg.harq_id;
          // Force CRC=OK for test UE.
          crc_pdu.tb_crc_success = true;
          // Force UL SINR.
          crc_pdu.ul_sinr_dB = 100;
        }

        // Forward CRC to the real MAC.
        forward_crc_ind_to_mac(crc_ind);
      }

      // Handle auto-generation of pending UCI indications.
      mac_uci_indication_message uci_ind;
      uci_ind.sl_rx = sl_rx;

      // > Handle pending PUCCHs.
      for (const pucch_info& pucch : entry->pucchs) {
        uci_ind.ucis.emplace_back(create_uci_pdu(pucch, test_ue_cfg));
      }

      // > Handle pending PUSCHs.
      for (const ul_sched_info& pusch : entry->puschs) {
        if (pusch.uci.has_value()) {
          uci_ind.ucis.emplace_back(create_uci_pdu(pusch, test_ue_cfg));
        }
      }

      // Forward UCI indication to real MAC.
      forward_uci_ind_to_mac(uci_ind);
    } else {
      logger.warning("TEST_MODE: Failed to generate UCI and CRC for slot={}. Cause: Overflow of the test mode internal "
                     "storage detected",
                     sl_rx);
    }
  }

  slot_handler.handle_slot_indication(context);
}

void mac_test_mode_cell_adapter::handle_error_indication(slot_point sl_tx, error_event event)
{
  slot_handler.handle_error_indication(sl_tx, event);

  if (event.pusch_and_pucch_discarded) {
    // Dispatch clearing of the history to the slot ind executor.
    if (not event_handler.schedule(cell_index, [this, sl_tx]() { history.clear(sl_tx); })) {
      logger.warning("TEST_MODE: Failed to handle error indication for slot={}. Cause: Overflow of the test mode "
                     "internal storage detected",
                     sl_tx);
    }
  }
}

void mac_test_mode_cell_adapter::handle_stop_indication()
{
  slot_handler.handle_stop_indication();
}

void mac_test_mode_cell_adapter::handle_crc(const mac_crc_indication_message& msg)
{
  mac_crc_indication_message msg_copy = msg;

  if (not test_ue_cfg.auto_ack_indication_delay.has_value()) {
    // Acquire history.
    const auto entry = history.read(msg.sl_rx);
    if (entry != nullptr) {
      // Forward CRC to MAC, but remove the UCI for the test mode UE.
      for (mac_crc_pdu& crc : msg_copy.crcs) {
        if (ue_info_mgr.is_cell_test_ue(cell_index, crc.rnti)) {
          // test mode UE case.

          // Find respective PUSCH PDU that was previously scheduled.
          bool found = std::any_of(entry->puschs.begin(), entry->puschs.end(), [&](const ul_sched_info& pusch) {
            return pusch.pusch_cfg.rnti == crc.rnti and pusch.pusch_cfg.harq_id == crc.harq_id;
          });
          if (not found) {
            logger.warning("TEST_MODE c-rnti={}: Failed to set CRC value for slot={}. Cause: Mismatch between provided "
                           "CRC and expected PUSCH",
                           crc.rnti,
                           msg.sl_rx);
            continue;
          }

          // Intercept the CRC indication and force crc=OK and UL SNR.
          crc.tb_crc_success = true;
          crc.ul_sinr_dB     = 100;
        }
      }
    } else {
      // Failed to lock entry in ring.
      logger.warning("TEST_MODE: Unable to set CRC value for slot={}. Cause: Overflow detected in test "
                     "mode internal storage",
                     msg.sl_rx);
    }
  } else {
    // In case of auto-ACK mode, test mode UEs are removed from CRC.
    msg_copy.crcs.erase(
        std::remove_if(msg_copy.crcs.begin(),
                       msg_copy.crcs.end(),
                       [this](const auto& crc) { return ue_info_mgr.is_cell_test_ue(cell_index, crc.rnti); }),
        msg_copy.crcs.end());
  }

  // Forward resulting CRC indication to real MAC.
  forward_crc_ind_to_mac(msg_copy);
}

void mac_test_mode_cell_adapter::forward_uci_ind_to_mac(const mac_uci_indication_message& uci_msg)
{
  if (uci_msg.ucis.empty()) {
    return;
  }

  // Forward UCI indication to real MAC.
  adapted.handle_uci(uci_msg);
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
    if (not ue_info_mgr.is_cell_test_ue(cell_index, pdu.rnti)) {
      continue;
    }

    auto rx_pdu = create_test_pdu_with_bsr(cell_index, crc_msg.sl_rx, pdu.rnti, to_harq_id(pdu.harq_id));
    if (not rx_pdu.has_value()) {
      logger.warning("TEST_MODE c-rnti={}: Unable to create test PDU with BSR", pdu.rnti);
      continue;
    }
    // In case of test mode UE, auto-forward a positive BSR.
    pdu_handler.handle_rx_data_indication(std::move(rx_pdu.value()));
  }
}

void mac_test_mode_cell_adapter::handle_uci(const mac_uci_indication_message& msg)
{
  mac_uci_indication_message msg_copy{msg};

  if (not test_ue_cfg.auto_ack_indication_delay.has_value()) {
    const auto entry = history.read(msg.sl_rx);
    if (entry != nullptr) {
      // Forward UCI to MAC, but alter the UCI for the test mode UE.
      for (mac_uci_pdu& test_uci : msg_copy.ucis) {
        if (ue_info_mgr.is_cell_test_ue(cell_index, test_uci.rnti)) {
          bool entry_found = false;
          if (std::holds_alternative<mac_uci_pdu::pusch_type>(test_uci.pdu)) {
            for (const ul_sched_info& pusch : entry->puschs) {
              if (pusch.pusch_cfg.rnti == test_uci.rnti and pusch.uci.has_value()) {
                test_uci    = create_uci_pdu(pusch, test_ue_cfg);
                entry_found = true;
              }
            }
          } else {
            // PUCCH case.
            for (const pucch_info& pucch : entry->pucchs) {
              if (pucch_info_and_uci_ind_match(pucch, test_uci)) {
                // Intercept the UCI indication and force HARQ-ACK=ACK and UCI.
                test_uci    = create_uci_pdu(pucch, test_ue_cfg);
                entry_found = true;
              }
            }
          }

          if (not entry_found) {
            logger.warning(
                "TEST_MODE c-rnti={}: Failed to set UCI value for slot={}. Cause: Mismatch between provided UCI and "
                "expected UCI",
                test_uci.rnti,
                msg.sl_rx);
          }
        }
      }
    } else {
      // Failed to lock entry in history ring.
      logger.warning("TEST_MODE: Unable to set UCI value for slot={}. Cause: Overflow detected in test mode "
                     "internal storage",
                     msg.sl_rx);
    }
  } else {
    // In case of auto-ACK mode, test mode UEs are removed from UCI.
    msg_copy.ucis.erase(
        std::remove_if(msg_copy.ucis.begin(),
                       msg_copy.ucis.end(),
                       [this](const auto& u) { return ue_info_mgr.is_cell_test_ue(cell_index, u.rnti); }),
        msg_copy.ucis.end());
  }

  // Forward UCI indication to real MAC.
  forward_uci_ind_to_mac(msg_copy);
}

void mac_test_mode_cell_adapter::handle_srs(const mac_srs_indication_message& msg)
{
  // TODO: Implement this method.
}

// Intercepts the sched + signalling results coming from the MAC.
void mac_test_mode_cell_adapter::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  if (last_slot_ind != dl_res.slot) {
    // Process any pending tasks for the test mode asynchronously.
    event_handler.process_pending_tasks(cell_index);
    last_slot_ind = dl_res.slot;
  }

  for (auto& grant : dl_res.dl_res->ue_grants) {
    rnti_t crnti = grant.pdsch_cfg.rnti;
    if (not ue_info_mgr.is_cell_test_ue(cell_index, crnti) or ue_info_mgr.is_msg4_rxed(crnti)) {
      // UE is not test mode or it has already received Msg4.
      continue;
    }

    // In case of SRB PDU received, we assume that the Msg4 is received. At this point, we update the test UE with
    // positive DL buffer states and BSR.
    auto& lchs = grant.tb_list[0].lc_chs_to_sched;
    if (std::any_of(
            lchs.begin(), lchs.end(), [](const auto& lc) { return lc.lcid.is_sdu() and is_srb(lc.lcid.to_lcid()); })) {
      if (test_ue_cfg.pdsch_active) {
        // Update DL buffer state automatically.
        dl_bs_notifier(crnti);
      }

      if (test_ue_cfg.pusch_active) {
        auto rx_pdu = create_test_pdu_with_bsr(cell_index, dl_res.slot, crnti, to_harq_id(0));
        if (not rx_pdu.has_value()) {
          logger.warning("TEST_MODE c-rnti={}: Unable to create test PDU with BSR", crnti);
          continue;
        }
        // In case of PUSCH test mode is enabled, push a BSR to trigger the first PUSCH.
        pdu_handler.handle_rx_data_indication(std::move(rx_pdu.value()));
      }

      // Mark Msg4 received for the UE.
      ue_info_mgr.msg4_rxed(crnti, true);

      // Push an UL PDU that will serve as rrcSetupComplete and get the UE out of fallback mode.
      auto rx_pdu = create_test_pdu_with_rrc_setup_complete(cell_index, dl_res.slot, crnti, to_harq_id(0));
      pdu_handler.handle_rx_data_indication(std::move(rx_pdu.value()));
    }
  }

  // Dispatch result to lower layers.
  result_notifier.on_new_downlink_scheduler_results(dl_res);
}

// Intercepts the UL results coming from the MAC.
void mac_test_mode_cell_adapter::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  if (last_slot_ind != ul_res.slot) {
    // Process any pending tasks for the test mode asynchronously.
    event_handler.process_pending_tasks(cell_index);
    last_slot_ind = ul_res.slot;
  }

  // Update history.
  {
    auto entry = history.write(ul_res.slot);
    if (entry != nullptr) {
      // Fill the ring element with the scheduler decisions.
      for (const pucch_info& pucch : ul_res.ul_res->pucchs) {
        if (ue_info_mgr.is_cell_test_ue(cell_index, pucch.crnti)) {
          entry->pucchs.push_back(pucch);
        }
      }
      for (const ul_sched_info& pusch : ul_res.ul_res->puschs) {
        if (ue_info_mgr.is_cell_test_ue(cell_index, pusch.pusch_cfg.rnti)) {
          entry->puschs.push_back(pusch);
        }
      }
    } else {
      logger.warning("TEST_MODE: Unable to write UL results for slot={}. Cause: Overflow detected in test mode "
                     "internal storage",
                     ul_res.slot);
    }
  }

  // Forward results to PHY.
  result_notifier.on_new_uplink_scheduler_results(ul_res);
}

void mac_test_mode_cell_adapter::on_cell_results_completion(slot_point slot)
{
  // Wait in slots between consecutive UL-CCCH messages to create test mode UEs.
  static constexpr unsigned wait_between_ul_ccchs = 5;

  // Forward results to lower layers.
  result_notifier.on_cell_results_completion(slot);

  // Check if more test mode UEs need to be created.
  // Note: The UE creation should only start after last_slot_ind is valid, i.e., after we have the guarantee that
  // the cell is active.
  if (nof_test_ues_created < test_ue_cfg.nof_ues and last_slot_ind.valid() and
      slot.count() % wait_between_ul_ccchs == 0) {
    auto ulcch_buf = byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00});
    if (not ulcch_buf.has_value()) {
      logger.warning("TEST_MODE: Postponing creation of test mode ue={}. Cause: Unable to allocate byte_buffer for "
                     "UL-CCCH message",
                     nof_test_ues_created);
    }

    const rnti_t test_ue_rnti =
        to_rnti(to_value(test_ue_cfg.rnti) + (cell_index * test_ue_cfg.nof_ues) + nof_test_ues_created);

    // Inject UL-CCCH message that will trigger the test mode UE creation.
    pdu_handler.handle_rx_data_indication(
        mac_rx_data_indication{slot, cell_index, {mac_rx_pdu{test_ue_rnti, 0, 0, ulcch_buf.value().copy()}}});

    // UL-CCCH message injected. Update counter.
    logger.info("TEST_MODE: Starting UE with rnti={} creation on cell={}. There still {}/{} left to be created.",
                test_ue_rnti,
                fmt::underlying(cell_index),
                test_ue_cfg.nof_ues - (nof_test_ues_created + 1),
                test_ue_cfg.nof_ues);
    nof_test_ues_created++;
  }
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

mac_test_mode_adapter::mac_test_mode_adapter(const srs_du::du_test_mode_config::test_mode_ue_config& test_ue_cfg_,
                                             mac_result_notifier&                                    phy_notifier_,
                                             unsigned                                                nof_cells) :
  test_ue(test_ue_cfg_),
  event_handler(nof_cells),
  ue_info_mgr(event_handler, test_ue.rnti, test_ue.nof_ues, nof_cells),
  phy_notifier(std::make_unique<phy_test_mode_adapter>(phy_notifier_))
{
}

mac_test_mode_adapter::~mac_test_mode_adapter() = default;

void mac_test_mode_adapter::connect(std::unique_ptr<mac_interface> mac_ptr)
{
  mac_adapted = std::move(mac_ptr);
}

mac_cell_controller& mac_test_mode_adapter::add_cell(const mac_cell_creation_request& cell_cfg)
{
  // Create cell in real MAC.
  mac_cell_controller& cell = mac_adapted->get_cell_manager().add_cell(cell_cfg);

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
                                                   event_handler,
                                                   ue_info_mgr);

  if (cell_info_handler.size() <= cell_cfg.cell_index) {
    cell_info_handler.resize(cell_cfg.cell_index + 1);
  }
  cell_info_handler[cell_cfg.cell_index] = std::move(new_cell);

  phy_notifier->connect(cell_cfg.cell_index, *cell_info_handler[cell_cfg.cell_index]);

  return cell;
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

mac_cell_time_mapper& mac_test_mode_adapter::get_time_mapper(du_cell_index_t cell_index)
{
  return mac_adapted->get_cell_manager().get_time_mapper(cell_index);
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
mac_test_mode_adapter::adapt_bearers(const std::vector<mac_logical_channel_config>& orig_bearers) const
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
    cfg_copy.initial_fallback      = true;

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

void mac_test_mode_adapter::handle_ue_config_applied(du_ue_index_t ue_idx)
{
  mac_adapted->get_ue_configurator().handle_ue_config_applied(ue_idx);
}

std::unique_ptr<mac_interface> srsran::srs_du::create_du_high_mac(const mac_config&                  mac_cfg,
                                                                  const srs_du::du_test_mode_config& test_cfg,
                                                                  unsigned                           nof_cells)
{
  if (not test_cfg.test_ue.has_value()) {
    return create_mac(mac_cfg);
  }

  // Create a MAC test mode adapter that wraps the real MAC.
  auto mac_testmode = std::make_unique<mac_test_mode_adapter>(*test_cfg.test_ue, mac_cfg.phy_notifier, nof_cells);
  mac_testmode->connect(create_mac(mac_config{mac_cfg.ul_ccch_notifier,
                                              mac_cfg.ue_exec_mapper,
                                              mac_cfg.cell_exec_mapper,
                                              mac_cfg.ctrl_exec,
                                              mac_testmode->get_phy_notifier(),
                                              mac_cfg.mac_cfg,
                                              mac_cfg.pcap,
                                              mac_cfg.timers,
                                              mac_cfg.metrics,
                                              mac_cfg.sched_cfg}));
  return mac_testmode;
}

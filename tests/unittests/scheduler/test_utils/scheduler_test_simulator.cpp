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

#include "scheduler_test_simulator.h"
#include "../test_utils/indication_generators.h"
#include "config_generators.h"
#include "sched_random_utils.h"
#include "scheduler_test_suite.h"

using namespace srsran;

scheduler_expert_config srsran::make_custom_scheduler_expert_config(bool enable_csi_rs_pdsch_multiplexing)
{
  scheduler_expert_config exp_cfg             = config_helpers::make_default_scheduler_expert_config();
  exp_cfg.ue.enable_csi_rs_pdsch_multiplexing = enable_csi_rs_pdsch_multiplexing;
  return exp_cfg;
}

scheduler_test_simulator::scheduler_test_simulator(const scheduler_expert_config& sched_cfg_,
                                                   unsigned                       tx_rx_delay_,
                                                   subcarrier_spacing             max_scs) :
  tx_rx_delay(tx_rx_delay_),
  logger([]() -> srslog::basic_logger& {
    srslog::init();
    auto& l = srslog::fetch_basic_logger("SCHED", true);
    l.set_level(srslog::basic_levels::debug);
    return l;
  }()),
  test_logger(srslog::fetch_basic_logger("TEST", true)),
  sched_cfg(sched_cfg_),
  sched(create_scheduler(scheduler_config{sched_cfg, notif})),
  next_slot(test_helper::generate_random_slot_point(max_scs))
{
  test_logger.set_level(srslog::basic_levels::debug);
  logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());
  srslog::flush();
}

scheduler_test_simulator::scheduler_test_simulator(unsigned           tx_rx_delay_,
                                                   subcarrier_spacing max_scs,
                                                   bool               enable_csi_rs_pdsch_multiplexing) :
  scheduler_test_simulator(make_custom_scheduler_expert_config(enable_csi_rs_pdsch_multiplexing), tx_rx_delay_, max_scs)
{
}

scheduler_test_simulator::scheduler_test_simulator(const scheduler_test_sim_config& cfg) :
  scheduler_test_simulator(cfg.sched_cfg, cfg.tx_rx_delay, cfg.max_scs)
{
  auto_uci = cfg.auto_uci;
  auto_crc = cfg.auto_crc;
}

scheduler_test_simulator::~scheduler_test_simulator()
{
  // Let any pending allocations complete.
  const unsigned max_k = std::max(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1, SCHEDULER_MAX_K2);
  for (unsigned i = 0; i < max_k; i++) {
    run_slot();
  }

  // Flush logs before exiting.
  srslog::flush();
}

void scheduler_test_simulator::add_cell(const sched_cell_configuration_request_message& cell_cfg_req)
{
  cell_cfg_list.emplace_back(sched_cfg, cell_cfg_req);
  last_sched_res_list.emplace_back(nullptr);
  sched->handle_cell_configuration_request(cell_cfg_req);
}

void scheduler_test_simulator::add_ue(const sched_ue_creation_request_message& ue_request, bool wait_notification)
{
  static const size_t ADD_TIMEOUT = 100;
  sched->handle_ue_creation_request(ue_request);
  if (wait_notification) {
    notif.last_ue_index_cfg.reset();
    for (unsigned i = 0; i != ADD_TIMEOUT and notif.last_ue_index_cfg != ue_request.ue_index; ++i) {
      run_slot();
    }
  }
  rnti_to_ue_index.insert(std::make_pair(ue_request.crnti, ue_request.ue_index));
}

void scheduler_test_simulator::rem_ue(du_ue_index_t ue_index)
{
  sched->handle_ue_removal_request(ue_index);
  for (auto it = rnti_to_ue_index.begin(); it != rnti_to_ue_index.end(); ++it) {
    if (it->second == ue_index) {
      rnti_to_ue_index.erase(it);
      break;
    }
  }
}

void scheduler_test_simulator::push_dl_buffer_state(const dl_buffer_state_indication_message& upd)
{
  sched->handle_dl_buffer_state_indication(upd);
}

void scheduler_test_simulator::push_uci_indication(du_cell_index_t cell_idx, slot_point slot_uci)
{
  uci_indication uci;
  uci.cell_index = cell_idx;
  uci.slot_rx    = slot_uci;

  uci_indication::uci_pdu uci_pdu{};
  auto&                   uci_pucch_f1 = uci_pdu.pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>();
  uci_pucch_f1.sr_detected             = false;
  uci_pucch_f1.harqs.emplace_back(mac_harq_ack_report_status::ack);
  uci.ucis.emplace_back(uci_pdu);

  sched->handle_uci_indication(uci);
}

void scheduler_test_simulator::run_slot(du_cell_index_t cell_idx)
{
  srsran_assert(cell_cfg_list.size() > cell_idx, "Invalid cellId={}", fmt::underlying(cell_idx));
  logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

  last_sched_res_list[cell_idx] = &sched->slot_indication(next_slot, cell_idx);

  // Ensure the scheduler result is consistent with the cell configuration and there are no collisions.
  test_scheduler_result_consistency(cell_cfg_list[cell_idx], next_slot, *last_sched_res_list[cell_idx]);

  // In case auto-feedback is enabled, handle it.
  handle_auto_feedback(cell_idx);

  ++next_slot;
}

bool scheduler_test_simulator::run_slot_until(const std::function<bool()>& cond_func, unsigned slot_timeout)
{
  unsigned count = 0;
  for (; count < slot_timeout; ++count) {
    run_slot();
    if (cond_func()) {
      break;
    }
  }
  return count < slot_timeout;
}

void scheduler_test_simulator::handle_auto_feedback(du_cell_index_t cell_idx)
{
  if (not auto_uci and not auto_crc) {
    return;
  }
  // Note: next_slot hasn't been incremented yet.
  slot_point sl_rx = next_slot;

  uci_indication uci_ind;
  uci_ind.cell_index = cell_idx;
  uci_ind.slot_rx    = sl_rx;
  ul_crc_indication crc_ind;
  crc_ind.cell_index = cell_idx;
  crc_ind.sl_rx      = sl_rx;

  if (auto_uci) {
    // Handle PUCCHs.
    for (const pucch_info& pucch : this->last_sched_res_list[cell_idx]->ul.pucchs) {
      if (pucch.format() == pucch_format::FORMAT_1 and pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
        // Skip SRs.
        continue;
      }

      const du_ue_index_t ue_idx = rnti_to_ue_index.at(pucch.crnti);
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_idx, pucch));
    }
  }

  for (const auto& pusch : this->last_sched_res_list[cell_idx]->ul.puschs) {
    if (auto_uci and pusch.uci.has_value()) {
      const du_ue_index_t ue_idx = pusch.context.ue_index;
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(pusch.pusch_cfg.rnti, ue_idx, pusch.uci.value()));
    }

    if (auto_crc) {
      crc_ind.crcs.push_back(test_helper::create_crc_pdu_indication(pusch));
    }
  }

  // Forward indications to the scheduler.
  if (not uci_ind.ucis.empty()) {
    this->sched->handle_uci_indication(uci_ind);
  }
  if (not crc_ind.crcs.empty()) {
    this->sched->handle_crc_indication(crc_ind);
  }
}

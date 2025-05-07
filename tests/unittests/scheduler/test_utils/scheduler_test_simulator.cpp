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
}

void scheduler_test_simulator::rem_ue(du_ue_index_t ue_index)
{
  sched->handle_ue_removal_request(ue_index);
}

void scheduler_test_simulator::push_dl_buffer_state(const dl_buffer_state_indication_message& upd)
{
  sched->handle_dl_buffer_state_indication(upd);
}

void scheduler_test_simulator::run_slot(du_cell_index_t cell_idx)
{
  srsran_assert(cell_cfg_list.size() > cell_idx, "Invalid cellId={}", fmt::underlying(cell_idx));
  logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

  last_sched_res_list[cell_idx] = &sched->slot_indication(next_slot, cell_idx);

  test_scheduler_result_consistency(cell_cfg_list[cell_idx], next_slot, *last_sched_res_list[cell_idx]);
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

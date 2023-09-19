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

#include "dummy_test_components.h"
#include "lib/scheduler/cell/cell_configuration.h"
#include "scheduler_test_suite.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_factory.h"
#include "srsran/support/test_utils.h"

namespace srsran {

/// Helper class to help setup a scheduler unit test.
class scheduler_test_bench
{
public:
  explicit scheduler_test_bench(unsigned tx_rx_delay_ = 4, subcarrier_spacing max_scs = subcarrier_spacing::kHz15) :
    tx_rx_delay(tx_rx_delay_),
    logger([]() -> srslog::basic_logger& {
      srslog::init();
      auto& l = srslog::fetch_basic_logger("SCHED", true);
      l.set_level(srslog::basic_levels::debug);
      return l;
    }()),
    sched(create_scheduler(scheduler_config{sched_cfg, notif, metric_notif})),
    next_slot(test_helpers::generate_random_slot_point(max_scs))
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    srslog::flush();
  }

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  slot_point last_result_slot() const { return next_slot - 1; }

  void add_cell(const sched_cell_configuration_request_message& cell_cfg_req)
  {
    cell_cfg_list.emplace(cell_cfg_req.cell_index, sched_cfg, cell_cfg_req);
    sched->handle_cell_configuration_request(cell_cfg_req);
  }

  void add_ue(const sched_ue_creation_request_message& ue_request, bool wait_notification = false)
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

  void rem_ue(du_ue_index_t ue_index) { sched->handle_ue_removal_request(ue_index); }

  void push_dl_buffer_state(const dl_buffer_state_indication_message& upd)
  {
    sched->handle_dl_buffer_state_indication(upd);
  }

  void push_bsr(const ul_bsr_indication_message& bsr) { sched->handle_ul_bsr_indication(bsr); }

  void run_slot(du_cell_index_t cell_idx = to_du_cell_index(0))
  {
    srsran_assert(cell_cfg_list.size() > cell_idx, "Invalid cellId={}", cell_idx);
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    last_sched_res = &sched->slot_indication(next_slot, cell_idx);
    test_scheduler_result_consistency(cell_cfg_list[cell_idx], next_slot, *last_sched_res);
    ++next_slot;
  }

  template <typename StopCondition>
  bool run_slot_until(const StopCondition& cond_func, unsigned slot_timeout = 1000)
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

  const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti) const
  {
    for (unsigned i = 0; i != last_sched_res->dl.dl_pdcchs.size(); ++i) {
      if (last_sched_res->dl.dl_pdcchs[i].ctx.rnti == rnti) {
        return &last_sched_res->dl.dl_pdcchs[i];
      }
    }
    return nullptr;
  }

  const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti) const
  {
    for (unsigned i = 0; i != last_sched_res->dl.ul_pdcchs.size(); ++i) {
      if (last_sched_res->dl.ul_pdcchs[i].ctx.rnti == rnti) {
        return &last_sched_res->dl.ul_pdcchs[i];
      }
    }
    return nullptr;
  }

  const unsigned                      tx_rx_delay;
  srslog::basic_logger&               logger;
  const scheduler_expert_config       sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier            notif;
  scheduler_ue_metrics_dummy_notifier metric_notif;
  std::unique_ptr<mac_scheduler>      sched;

  slotted_array<cell_configuration, MAX_NOF_DU_CELLS> cell_cfg_list;

  slot_point          next_slot;
  const sched_result* last_sched_res = nullptr;
};

} // namespace srsran

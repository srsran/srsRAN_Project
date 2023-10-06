/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_scheduler_impl.h"
#include "../policy/scheduler_policy_factory.h"

using namespace srsran;

ue_scheduler_impl::ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_,
                                     sched_configuration_notifier&     mac_notif,
                                     scheduler_metrics_handler&        metric_handler,
                                     scheduler_event_logger&           sched_ev_logger) :
  expert_cfg(expert_cfg_),
  sched_strategy(create_scheduler_strategy(scheduler_strategy_params{"time_rr", &srslog::fetch_basic_logger("SCHED")})),
  ue_db(mac_notif),
  ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED")),
  event_mng(expert_cfg, ue_db, mac_notif, metric_handler, sched_ev_logger)
{
}

void ue_scheduler_impl::add_cell(const ue_scheduler_cell_params& params)
{
  ue_res_grid_view.add_cell(*params.cell_res_alloc);
  cells[params.cell_index] = std::make_unique<cell>(expert_cfg, params, ue_db);
  event_mng.add_cell(params.cell_res_alloc->cfg, cells[params.cell_index]->srb0_sched);
  ue_alloc.add_cell(params.cell_index, *params.pdcch_sched, *params.uci_alloc, *params.cell_res_alloc);
}

void ue_scheduler_impl::run_sched_strategy(slot_point slot_tx)
{
  // Update all UEs state.
  ue_db.slot_indication(slot_tx);

  if (not ue_res_grid_view.get_cell_cfg_common(to_du_cell_index(0)).is_dl_enabled(slot_tx)) {
    // This slot is inactive for PDCCH in this cell. We therefore, can skip the scheduling strategy.
    // Note: we are currently assuming that all cells have the same TDD pattern and that the scheduling strategy
    // only allocates PDCCHs for the current slot_tx.
    return;
  }

  // Perform round-robin prioritization of UL and DL scheduling. This gives unfair preference to DL over UL. This is
  // done to avoid the issue of sending wrong DAI value in DCI format 0_1 to UE while the PDSCH is allocated
  // right after allocating PUSCH in the same slot, resulting in gNB expecting 1 HARQ ACK bit to be multiplexed in
  // UCI in PUSCH and UE sending 4 HARQ ACK bits (DAI = 3).
  // Example: K1==K2=4 and PUSCH is allocated before PDSCH.
  if (expert_cfg.enable_csi_rs_pdsch_multiplexing or (*cells[0]->cell_res_alloc)[0].result.dl.csi_rs.empty()) {
    sched_strategy->dl_sched(ue_alloc, ue_res_grid_view, ue_db);
  }
  sched_strategy->ul_sched(ue_alloc, ue_res_grid_view, ue_db);
}

void ue_scheduler_impl::update_harq_pucch_counter(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  // We need to update the PUCCH counter after the SR/CSI scheduler because the allocation of CSI/SR can add/remove
  // PUCCH grants.
  const unsigned HARQ_SLOT_DELAY = 0;
  const auto&    slot_alloc      = cell_alloc[HARQ_SLOT_DELAY];

  rnti_pucch_cnt.clear();

  // Spans through the pucch grant list and counts the occurrences of the HARQ-ACK PUCCH grants for each RNTI.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    if ((pucch.format == pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0) or
        (pucch.format == pucch_format::FORMAT_2 and pucch.format_2.harq_ack_nof_bits > 0)) {
      if (rnti_pucch_cnt.find(pucch.crnti) == rnti_pucch_cnt.end()) {
        rnti_pucch_cnt.emplace(pucch.crnti, 1);
      } else {
        ++rnti_pucch_cnt[pucch.crnti];
      }
    }
  }

  // For the RNTI found above, update the PUCCH counter only for the dl_harq process which expects an ack for this slot.
  for (auto& user : ue_db) {
    if (rnti_pucch_cnt.find(user->get_pcell().rnti()) != rnti_pucch_cnt.end()) {
      for (unsigned h_pid = 0; h_pid != user->get_pcell().harqs.nof_dl_harqs(); ++h_pid) {
        dl_harq_process& h_dl = user->get_pcell().harqs.dl_harq(h_pid);
        if (h_dl.is_waiting_ack() and h_dl.slot_ack() == slot_alloc.slot) {
          h_dl.update_pucch_counter(rnti_pucch_cnt[user->get_pcell().rnti()]);
        }
      }
    }
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Run cell-specific SRB0 scheduler.
  cells[cell_index]->srb0_sched.run_slot(*cells[cell_index]->cell_res_alloc);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, ue_alloc.nof_cells(), [this, slot_tx]() { run_sched_strategy(slot_tx); });

  // Schedule UCI as the last step.
  cells[cell_index]->uci_sched.run_slot(*cells[cell_index]->cell_res_alloc, slot_tx);

  // We need to update the PUCCH counter after the UCI scheduler because the allocation of CSI/SR can add/remove
  // PUCCH grants.
  update_harq_pucch_counter(*cells[cell_index]->cell_res_alloc, slot_tx);
}

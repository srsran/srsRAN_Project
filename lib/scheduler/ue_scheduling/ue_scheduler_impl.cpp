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
  event_mng(expert_cfg, ue_db, mac_notif, metric_handler, sched_ev_logger),
  logger(srslog::fetch_basic_logger("SCHED"))
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

void ue_scheduler_impl::update_harq_pucch_counter(cell_resource_allocator& cell_alloc)
{
  // We need to update the PUCCH counter after the SR/CSI scheduler because the allocation of CSI/SR can add/remove
  // PUCCH grants.
  const unsigned HARQ_SLOT_DELAY = 0;
  const auto&    slot_alloc      = cell_alloc[HARQ_SLOT_DELAY];

  // Spans through the PUCCH grant list and update the HARQ-ACK PUCCH grant counter for the corresponding RNTI and HARQ
  // process id.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    if ((pucch.format == pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0) or
        (pucch.format == pucch_format::FORMAT_2 and pucch.format_2.harq_ack_nof_bits > 0)) {
      ue* user = ue_db.find_by_rnti(pucch.crnti);
      // This is to handle the case of a UE that gets removed after the PUCCH gets allocated and before this PUCCH is
      // expected to be sent.
      if (user == nullptr) {
        logger.warning(
            "rnti={:#x}: No user with such RNTI found in the ue scheduler database. Skipping PUCCH grant counter",
            pucch.crnti,
            slot_alloc.slot);
        continue;
      }
      srsran_assert(pucch.format == pucch_format::FORMAT_1 or pucch.format == pucch_format::FORMAT_2,
                    "rnti={:#x}: Only PUCCH format 1 and format 2 are supported",
                    pucch.crnti);
      const unsigned nof_harqs_per_rnti_per_slot =
          pucch.format == pucch_format::FORMAT_1 ? pucch.format_1.harq_ack_nof_bits : pucch.format_2.harq_ack_nof_bits;
      // Each PUCCH grants can potentially carry ACKs for different HARQ processes (as many as the harq_ack_nof_bits)
      // expecting to be acknowledged on the same slot.
      for (unsigned harq_bit_idx = 0; harq_bit_idx != nof_harqs_per_rnti_per_slot; ++harq_bit_idx) {
        dl_harq_process* h_dl = user->get_pcell().harqs.find_dl_harq_waiting_ack_slot(slot_alloc.slot, harq_bit_idx);
        if (h_dl == nullptr) {
          logger.warning(
              "ue={} rnti={:#x}: No DL HARQ process with state waiting-for-ack found at slot={} for harq-bit-index",
              user->ue_index,
              user->crnti,
              slot_alloc.slot,
              harq_bit_idx);
          continue;
        };
        h_dl->increment_pucch_counter();
      }
    }
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Mark the start of a new slot in the UE grid allocator.
  ue_alloc.slot_indication();

  // Run cell-specific SRB0 scheduler.
  cells[cell_index]->srb0_sched.run_slot(*cells[cell_index]->cell_res_alloc);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, ue_alloc.nof_cells(), [this, slot_tx]() { run_sched_strategy(slot_tx); });

  // Schedule UCI as the last step.
  cells[cell_index]->uci_sched.run_slot(*cells[cell_index]->cell_res_alloc, slot_tx);

  // We need to update the PUCCH counter after the UCI scheduler because the allocation of CSI/SR can add/remove
  // PUCCH grants.
  update_harq_pucch_counter(*cells[cell_index]->cell_res_alloc);
}

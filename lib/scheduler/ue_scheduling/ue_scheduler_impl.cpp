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

#include "ue_scheduler_impl.h"
#include "../logging/scheduler_metrics_handler.h"

using namespace srsran;

ue_scheduler_impl::ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_) :
  expert_cfg(expert_cfg_),
  ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED")),
  event_mng(ue_db),
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

void ue_scheduler_impl::add_cell(const ue_scheduler_cell_params& params)
{
  ue_res_grid_view.add_cell(*params.cell_res_alloc);
  cells.emplace(params.cell_index, expert_cfg, params, ue_db, *params.cell_metrics);
  event_mng.add_cell(cell_creation_event{*params.cell_res_alloc,
                                         cells[params.cell_index].cell_harqs,
                                         cells[params.cell_index].fallback_sched,
                                         cells[params.cell_index].uci_sched,
                                         cells[params.cell_index].slice_sched,
                                         cells[params.cell_index].srs_sched,
                                         *params.cell_metrics,
                                         *params.ev_logger});
  ue_alloc.add_cell(params.cell_index, *params.pdcch_sched, *params.uci_alloc, *params.cell_res_alloc);
}

void ue_scheduler_impl::run_sched_strategy(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Update all UEs state.
  ue_db.slot_indication(slot_tx);

  if (not ue_res_grid_view.get_cell_cfg_common(cell_index).is_dl_enabled(slot_tx)) {
    // This slot is inactive for PDCCH in this cell. We therefore, can skip the scheduling strategy.
    // Note: we are currently assuming that all cells have the same TDD pattern and that the scheduling strategy
    // only allocates PDCCHs for the current slot_tx.
    return;
  }

  // Update slice context and compute slice priorities.
  cells[cell_index].slice_sched.slot_indication(slot_tx, ue_res_grid_view.get_grid(cell_index));

  // Perform round-robin prioritization of UL and DL scheduling. This gives unfair preference to DL over UL. This is
  // done to avoid the issue of sending wrong DAI value in DCI format 0_1 to UE while the PDSCH is allocated
  // right after allocating PUSCH in the same slot, resulting in gNB expecting 1 HARQ ACK bit to be multiplexed in
  // UCI in PUSCH and UE sending 4 HARQ ACK bits (DAI = 3).
  // Example: K1==K2=4 and PUSCH is allocated before PDSCH.
  if (expert_cfg.enable_csi_rs_pdsch_multiplexing or (*cells[cell_index].cell_res_alloc)[0].result.dl.csi_rs.empty()) {
    auto dl_slice_candidate = cells[cell_index].slice_sched.get_next_dl_candidate();
    while (dl_slice_candidate.has_value()) {
      auto&                           policy = cells[cell_index].slice_sched.get_policy(dl_slice_candidate->id());
      dl_slice_ue_cell_grid_allocator slice_pdsch_alloc{ue_alloc, *dl_slice_candidate};
      policy.dl_sched(
          slice_pdsch_alloc, ue_res_grid_view, *dl_slice_candidate, cells[cell_index].cell_harqs.pending_dl_retxs());
      dl_slice_candidate = cells[cell_index].slice_sched.get_next_dl_candidate();
    }
  }

  auto ul_slice_candidate = cells[cell_index].slice_sched.get_next_ul_candidate();
  while (ul_slice_candidate.has_value()) {
    auto&                           policy = cells[cell_index].slice_sched.get_policy(ul_slice_candidate->id());
    ul_slice_ue_cell_grid_allocator slice_pusch_alloc{ue_alloc, *ul_slice_candidate};
    policy.ul_sched(
        slice_pusch_alloc, ue_res_grid_view, *ul_slice_candidate, cells[cell_index].cell_harqs.pending_ul_retxs());
    ul_slice_candidate = cells[cell_index].slice_sched.get_next_ul_candidate();
  }
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
    if ((pucch.format == pucch_format::FORMAT_0 and pucch.format_0.harq_ack_nof_bits > 0) or
        (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0) or
        (pucch.format == pucch_format::FORMAT_2 and pucch.format_2.harq_ack_nof_bits > 0)) {
      ue* user = ue_db.find_by_rnti(pucch.crnti);
      // This is to handle the case of a UE that gets removed after the PUCCH gets allocated and before this PUCCH is
      // expected to be sent.
      if (user == nullptr) {
        logger.warning(
            "rnti={}: No user with such RNTI found in the ue scheduler database. Skipping PUCCH grant counter",
            pucch.crnti,
            slot_alloc.slot);
        continue;
      }
      unsigned nof_harqs_per_rnti_per_slot = 0;
      switch (pucch.format) {
        case pucch_format::FORMAT_0:
          nof_harqs_per_rnti_per_slot = pucch.format_0.harq_ack_nof_bits;
          break;
        case pucch_format::FORMAT_1:
          nof_harqs_per_rnti_per_slot = pucch.format_1.harq_ack_nof_bits;
          break;
        case pucch_format::FORMAT_2:
          nof_harqs_per_rnti_per_slot = pucch.format_2.harq_ack_nof_bits;
          break;
        default:
          srsran_assertion_failure("rnti={}: Only PUCCH format 0, 1 and 2 are supported", pucch.crnti);
      }
      // Each PUCCH grants can potentially carry ACKs for different HARQ processes (as many as the harq_ack_nof_bits)
      // expecting to be acknowledged on the same slot.
      for (unsigned harq_bit_idx = 0; harq_bit_idx != nof_harqs_per_rnti_per_slot; ++harq_bit_idx) {
        std::optional<dl_harq_process_handle> h_dl =
            user->get_pcell().harqs.find_dl_harq_waiting_ack(slot_alloc.slot, harq_bit_idx);
        if (not h_dl.has_value() or not h_dl->is_waiting_ack()) {
          logger.warning(
              "ue={} rnti={}: No DL HARQ process with state waiting-for-ack found at slot={} for harq-bit-index={}",
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

void ue_scheduler_impl::puxch_grant_sanitizer(cell_resource_allocator& cell_alloc)
{
  const unsigned HARQ_SLOT_DELAY = 0;
  const auto&    slot_alloc      = cell_alloc[HARQ_SLOT_DELAY];

  if (not cell_alloc.cfg.is_ul_enabled(slot_alloc.slot)) {
    return;
  }

  // Spans through the PUCCH grant list and check if there is any PUCCH grant scheduled for a UE that has a PUSCH.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    const auto* pusch_grant =
        std::find_if(slot_alloc.result.ul.puschs.begin(),
                     slot_alloc.result.ul.puschs.end(),
                     [&pucch](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == pucch.crnti; });

    if (pusch_grant != slot_alloc.result.ul.puschs.end()) {
      unsigned harq_bits = 0;
      unsigned csi_bits  = 0;
      unsigned sr_bits   = 0;
      if (pucch.format == pucch_format::FORMAT_1) {
        harq_bits = pucch.format_1.harq_ack_nof_bits;
        sr_bits   = sr_nof_bits_to_uint(pucch.format_1.sr_bits);
      } else if (pucch.format == pucch_format::FORMAT_2) {
        harq_bits = pucch.format_2.harq_ack_nof_bits;
        csi_bits  = pucch.format_2.csi_part1_bits;
        sr_bits   = sr_nof_bits_to_uint(pucch.format_2.sr_bits);
      }
      logger.error("rnti={}: has both PUCCH and PUSCH grants scheduled at slot {}, PUCCH  format={} with nof "
                   "harq-bits={} csi-1-bits={} sr-bits={}",
                   pucch.crnti,
                   slot_alloc.slot,
                   static_cast<unsigned>(pucch.format),
                   harq_bits,
                   csi_bits,
                   sr_bits);
    }
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx)
{
  std::unique_lock<std::mutex> lock(cell_group_mutex, std::defer_lock);
  if (cells.size() > 1) {
    // Only mutex if the cell group has more than one cell (Carrier Aggregation case).
    lock.lock();
  }
  if (last_sl_ind == slot_tx) {
    // This slot has already been processed by a cell of the same cell group.
    return;
  }
  last_sl_ind = slot_tx;

  for (auto& group_cell : cells) {
    du_cell_index_t cell_index = group_cell.cell_res_alloc->cfg.cell_index;

    // Process any pending events that are directed at UEs.
    event_mng.run(slot_tx, cell_index);

    // Mark the start of a new slot in the UE grid allocator.
    ue_alloc.slot_indication(slot_tx);

    // Check for timeouts in the cell HARQ processes.
    group_cell.cell_harqs.slot_indication(slot_tx);

    // Schedule periodic UCI (SR and CSI) before any UL grants.
    group_cell.uci_sched.run_slot(*group_cell.cell_res_alloc);

    // Schedule periodic SRS before any UE grants.
    cells[cell_index].srs_sched.run_slot(*group_cell.cell_res_alloc);

    // Run cell-specific SRB0 scheduler.
    group_cell.fallback_sched.run_slot(*group_cell.cell_res_alloc);

    // Run slice scheduler policies.
    run_sched_strategy(slot_tx, cell_index);

    // Update the PUCCH counter after the UE DL and UL scheduler.
    update_harq_pucch_counter(*group_cell.cell_res_alloc);

    // TODO: remove this.
    puxch_grant_sanitizer(*group_cell.cell_res_alloc);
  }
}

namespace {

class harq_manager_timeout_notifier : public harq_timeout_notifier
{
public:
  explicit harq_manager_timeout_notifier(cell_metrics_handler& metrics_handler_) : metrics_handler(metrics_handler_) {}

  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    metrics_handler.handle_harq_timeout(ue_idx, is_dl);
  }

private:
  cell_metrics_handler& metrics_handler;
};

} // namespace

ue_scheduler_impl::cell::cell(const scheduler_ue_expert_config& expert_cfg,
                              const ue_scheduler_cell_params&   params,
                              ue_repository&                    ues,
                              cell_metrics_handler&             metrics_handler) :
  cell_res_alloc(params.cell_res_alloc),
  cell_harqs(MAX_NOF_DU_UES, MAX_NOF_HARQS, std::make_unique<harq_manager_timeout_notifier>(metrics_handler)),
  uci_sched(params.cell_res_alloc->cfg, *params.uci_alloc, ues),
  fallback_sched(expert_cfg, params.cell_res_alloc->cfg, *params.pdcch_sched, *params.pucch_alloc, ues),
  slice_sched(params.cell_res_alloc->cfg, ues),
  srs_sched(params.cell_res_alloc->cfg, ues)
{
}

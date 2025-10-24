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

#include "ue_scheduler_impl.h"
#include "../logging/scheduler_metrics_handler.h"

using namespace srsran;

ue_scheduler_impl::ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_) :
  expert_cfg(expert_cfg_), logger(srslog::fetch_basic_logger("SCHED")), event_mng(ue_db)
{
}

ue_cell_scheduler* ue_scheduler_impl::do_add_cell(const ue_cell_scheduler_creation_request& params)
{
  cells.emplace(params.cell_index, *this, params);
  auto& cell = cells[params.cell_index];

  // Create a cell-specific UE event manager.
  cell.ev_mng = event_mng.add_cell(cell_creation_event{*params.cell_res_alloc,
                                                       cell.cell_harqs,
                                                       cell.ue_cell_db,
                                                       cell.fallback_sched,
                                                       cell.uci_sched,
                                                       cell.slice_sched,
                                                       cell.srs_sched,
                                                       *params.cell_metrics,
                                                       *params.ev_logger});

  return &cell;
}

void ue_scheduler_impl::do_start_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell reference not found in the scheduler");

  // Signal event manager that new events can be processed for this cell.
  cells[cell_index].ev_mng->start();
}

void ue_scheduler_impl::do_stop_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell reference not found in the scheduler");
  auto& c = cells[cell_index];

  // Halt any pending events associated with this cell.
  cells[cell_index].ev_mng->stop();

  // Stop sub-schedulers.
  c.fallback_sched.stop();
  c.srs_sched.stop();
  c.uci_sched.stop();
  c.cell_harqs.stop();

  // Remove UEs from the UE repository associated with this cell.
  ue_db.handle_cell_deactivation(cell_index);
}

void ue_scheduler_impl::do_rem_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell reference not found in the scheduler");

  do_stop_cell(cell_index);

  // Remove cell from UE scheduler.
  cells.erase(cell_index);
}

void ue_scheduler_impl::run_sched_strategy(du_cell_index_t cell_index)
{
  auto& cell = cells[cell_index];

  // Schedule DL first.
  // Note: DL should be scheduled first so that the right DAI value is picked in DCI format 0_1.
  while (auto dl_slice_candidate = cell.slice_sched.get_next_dl_candidate()) {
    scheduler_policy& policy = cell.slice_sched.get_policy(dl_slice_candidate->id());
    cell.intra_slice_sched.dl_sched(dl_slice_candidate.value(), policy);
  }

  while (auto ul_slice_candidate = cell.slice_sched.get_next_ul_candidate()) {
    scheduler_policy& policy = cell.slice_sched.get_policy(ul_slice_candidate->id());
    cell.intra_slice_sched.ul_sched(ul_slice_candidate.value(), policy);
  }
}

void ue_scheduler_impl::update_harq_pucch_counter(cell_resource_allocator& cell_alloc)
{
  // We need to update the PUCCH counter after the SR/CSI scheduler because the allocation of CSI/SR can add/remove
  // PUCCH grants.
  static constexpr unsigned HARQ_SLOT_DELAY = 0;
  const auto&               slot_alloc      = cell_alloc[HARQ_SLOT_DELAY];

  // Spans through the PUCCH grant list and update the HARQ-ACK PUCCH grant counter for the corresponding RNTI and HARQ
  // process id.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    const unsigned nof_harqs_per_rnti_per_slot = pucch.uci_bits.harq_ack_nof_bits;
    if (nof_harqs_per_rnti_per_slot == 0) {
      continue;
    }

    ue* user = ue_db.find_by_rnti(pucch.crnti);
    // This is to handle the case of a UE that gets removed after the PUCCH gets allocated and before this PUCCH is
    // expected to be sent.
    if (user == nullptr) {
      logger.warning("rnti={}: No user with such RNTI found in the ue scheduler database. Skipping PUCCH grant counter",
                     pucch.crnti,
                     slot_alloc.slot);
      continue;
    }
    // Each PUCCH grants can potentially carry ACKs for different HARQ processes (as many as the harq_ack_nof_bits)
    // expecting to be acknowledged on the same slot.
    for (unsigned harq_bit_idx = 0; harq_bit_idx != nof_harqs_per_rnti_per_slot; ++harq_bit_idx) {
      std::optional<dl_harq_process_handle> h_dl =
          user->get_pcell().harqs.find_dl_harq_waiting_ack(slot_alloc.slot, harq_bit_idx);
      if (not h_dl.has_value() or not h_dl->is_waiting_ack()) {
        logger.warning(
            "ue={} rnti={}: No DL HARQ process with state waiting-for-ack found at slot={} for harq-bit-index={}",
            fmt::underlying(user->ue_index),
            user->crnti,
            slot_alloc.slot,
            harq_bit_idx);
        continue;
      };
      h_dl->increment_pucch_counter();
    }
  }
}

[[maybe_unused]] static bool puxch_grant_sanitizer(cell_resource_allocator& cell_alloc, srslog::basic_logger& logger)
{
  const unsigned HARQ_SLOT_DELAY = 0;
  const auto&    slot_alloc      = cell_alloc[HARQ_SLOT_DELAY];

  if (not cell_alloc.cfg.is_ul_enabled(slot_alloc.slot)) {
    return true;
  }

  // Spans through the PUCCH grant list and check if there is any PUCCH grant scheduled for a UE that has a PUSCH.
  for (const auto& pucch : slot_alloc.result.ul.pucchs) {
    const auto* pusch_grant =
        std::find_if(slot_alloc.result.ul.puschs.begin(),
                     slot_alloc.result.ul.puschs.end(),
                     [&pucch](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == pucch.crnti; });

    if (pusch_grant != slot_alloc.result.ul.puschs.end()) {
      logger.error("rnti={}: has both PUCCH and PUSCH grants scheduled at slot {}, PUCCH  format={} with uci_bits={}",
                   pucch.crnti,
                   slot_alloc.slot,
                   static_cast<unsigned>(pucch.format()),
                   pucch.uci_bits);
      return false;
    }
  }
  return true;
}

void ue_scheduler_impl::run_slot_impl(slot_point slot_tx)
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
    group_cell.ev_mng->run_slot(slot_tx);

    // Update all UEs state.
    ue_db.slot_indication(slot_tx);

    // Check for timeouts in the cell HARQ processes.
    group_cell.cell_harqs.slot_indication(slot_tx);

    // Schedule periodic UCI (SR and CSI) before any UL grants.
    group_cell.uci_sched.run_slot(*group_cell.cell_res_alloc);

    // Schedule periodic SRS before any UE grants.
    group_cell.srs_sched.run_slot(*group_cell.cell_res_alloc);

    // Run cell-specific SRB0 scheduler.
    group_cell.fallback_sched.run_slot(*group_cell.cell_res_alloc);

    // Update slice context and compute slice priorities.
    group_cell.slice_sched.slot_indication(slot_tx, *group_cell.cell_res_alloc);

    // Update intra-slice scheduler context.
    group_cell.intra_slice_sched.slot_indication(slot_tx);

    // Run slice scheduler policies.
    run_sched_strategy(cell_index);

    // The post processing is done for DL and UL slots.
    group_cell.intra_slice_sched.post_process_results();

    // Update the PUCCH counter after the UE DL and UL scheduler.
    update_harq_pucch_counter(*group_cell.cell_res_alloc);

    srsran_sanity_check(puxch_grant_sanitizer(*group_cell.cell_res_alloc, logger),
                        "PUCCH and PUSCH found for the same UE in the same slot");
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

ue_scheduler_impl::cell_context::cell_context(ue_scheduler_impl&                        parent_,
                                              const ue_cell_scheduler_creation_request& params) :
  parent(parent_),
  cell_res_alloc(params.cell_res_alloc),
  cell_harqs(
      MAX_NOF_DU_UES,
      MAX_NOF_HARQS,
      std::make_unique<harq_manager_timeout_notifier>(*params.cell_metrics),
      std::make_unique<harq_manager_timeout_notifier>(*params.cell_metrics),
      parent.expert_cfg.dl_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_res_alloc->cfg.scs_common),
      parent.expert_cfg.ul_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_res_alloc->cfg.scs_common),
      cell_harq_manager::DEFAULT_ACK_TIMEOUT_SLOTS,
      params.cell_res_alloc->cfg.ntn_cs_koffset,
      params.cell_res_alloc->cfg.dl_harq_mode_b,
      params.cell_res_alloc->cfg.ul_harq_mode_b),
  ue_cell_db(parent.ue_db.add_cell(params.cell_index)),
  uci_sched(params.cell_res_alloc->cfg, *params.uci_alloc, parent.ue_db),
  fallback_sched(parent.expert_cfg,
                 params.cell_res_alloc->cfg,
                 *params.pdcch_sched,
                 *params.pucch_alloc,
                 *params.uci_alloc,
                 parent.ue_db),
  slice_sched(params.cell_res_alloc->cfg, parent.ue_db),
  intra_slice_sched(parent.expert_cfg,
                    parent.ue_db,
                    *params.pdcch_sched,
                    *params.uci_alloc,
                    *params.cell_res_alloc,
                    *params.cell_metrics,
                    cell_harqs,
                    srslog::fetch_basic_logger("SCHED")),
  srs_sched(params.cell_res_alloc->cfg, parent.ue_db)
{
}

ue_scheduler_impl::cell_context::~cell_context()
{
  parent.ue_db.rem_cell(ue_cell_db.cell_index());
}

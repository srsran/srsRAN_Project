/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_cell_grid_allocator.h"

namespace srsran {

class pdcch_resource_allocator;
class uci_allocator;
class scheduler_policy;

using ue_sched_priority                       = double;
const ue_sched_priority forbid_sched_priority = std::numeric_limits<ue_sched_priority>::min();

struct ue_pdsch_newtx_candidate {
  const slice_ue* ue;
  const ue_cell*  ue_cc;
  unsigned        pending_bytes;
  double          priority;
};

class intra_slice_scheduler
{
public:
  intra_slice_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                        ue_repository&                    ues_,
                        srslog::basic_logger&             logger_);

  void add_cell(du_cell_index_t           cell_index,
                pdcch_resource_allocator& pdcch_sched,
                uci_allocator&            uci_alloc,
                cell_resource_allocator&  cell_alloc,
                cell_harq_manager&        cell_harqs);

  void slot_indication(slot_point sl_tx);

  void dl_sched(slot_point                    sl_tx,
                du_cell_index_t               cell_index,
                const dl_ran_slice_candidate& candidate,
                scheduler_policy&             dl_policy);

private:
  struct cell_t {
    du_cell_index_t           cell_index;
    pdcch_resource_allocator* pdcch_sched;
    uci_allocator*            uci_alloc;
    cell_resource_allocator*  cell_alloc;
    cell_harq_manager*        cell_harqs;
  };

  bool can_allocate_pdsch(slot_point      sl_tx,
                          slot_point      sl_pdsch,
                          du_cell_index_t cell_index,
                          const slice_ue& u,
                          const ue_cell&  ue_cc) const;

  std::optional<ue_pdsch_newtx_candidate> create_newtx_dl_candidate(slot_point      sl_pdcch,
                                                                    slot_point      sl_pdsch,
                                                                    du_cell_index_t cell_index,
                                                                    const slice_ue& u) const;

  unsigned schedule_retx_candidates(du_cell_index_t               cell_index,
                                    const dl_ran_slice_candidate& slice,
                                    unsigned                      max_ue_grants_to_alloc);

  void schedule_newtx_candidates(du_cell_index_t               cell_index,
                                 const dl_ran_slice_candidate& slice,
                                 unsigned                      max_ue_grants_to_alloc);

  unsigned max_pdschs_to_alloc(slot_point pdcch_slot, slot_point pdsch_slot, du_cell_index_t cell_index);

  const scheduler_ue_expert_config& expert_cfg;
  ue_repository&                    ues;
  srslog::basic_logger&             logger;

  static_vector<cell_t, MAX_NOF_DU_CELLS> cells;

  ue_cell_grid_allocator ue_alloc;

  slot_point last_sl_tx;

  // Number of allocation attempts for DL in the given slot.
  unsigned dl_attempts_count = 0;

  std::vector<ue_pdsch_newtx_candidate> dl_newtx_candidates;
};

} // namespace srsran

/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_time_rr.h"

using namespace srsgnb;

/// @brief Algorithm to select next UE to allocate in a time-domain RR fashion
/// @param ue_db map of "slot_ue"
/// @param rr_count starting index to select next UE
/// @param p callable with signature "bool(slot_ue&)" that returns true if UE allocation was successful
/// @return true if a UE was allocated
template <typename Predicate>
bool round_robin_apply(const ue_list& ue_db, uint32_t rr_count, const Predicate& p)
{
  if (ue_db.empty()) {
    return false;
  }
  auto next_it = ue_db.lower_bound(to_du_ue_index(rr_count % MAX_NOF_DU_UES));
  for (uint32_t count = 0; count < ue_db.size(); ++count, ++next_it) {
    if (next_it == ue_db.end()) {
      // wrap-around
      next_it = ue_db.begin();
    }
    if (p(*next_it)) {
      return true;
    }
  }
  return false;
}

/// Allocate UE PDSCH grant.
bool alloc_dl_ue(const ue& u, ue_pdsch_allocator& pdsch_alloc, bool is_retx)
{
  ofdm_symbol_range pdsch_symbols{2, 14};

  // Prioritize PCell over SCells.
  for (const ue_carrier* ue_cc : u.ue_carriers()) {
    const dl_harq_process* h = nullptr;
    if (is_retx) {
      h = ue_cc->harqs.find_pending_dl_retx();
    } else if (u.has_pending_txs()) {
      h = ue_cc->harqs.find_empty_dl_harq();
    }
    if (h == nullptr) {
      continue;
    }
    const cell_slot_resource_grid& grid = pdsch_alloc.dl_resource_grid(ue_cc->cell_index, 0);
    for (const bwp_configuration* bwp : ue_cc->cfg().dl_bwps) {
      if (bwp == nullptr) {
        continue;
      }
      prb_bitmap   used_crbs     = grid.used_crbs(*bwp, pdsch_symbols);
      crb_interval ue_grant_crbs = find_empty_interval_of_length(used_crbs, bwp->crbs.length(), 0);
      if (not ue_grant_crbs.empty()) {
        pdsch_alloc.allocate_pdsch(ue_pdsch_grant{
            &u, ue_cc->cell_index, h, ue_grant_crbs, pdsch_symbols, 0, dci_dl_format::f1_0, aggregation_level::n4});
      }
    }
  }
  return false;
}

/// Allocate UE PDSCH grant.
bool alloc_ul_ue(const ue& u, ue_pusch_allocator& pusch_alloc, bool is_retx)
{
  ofdm_symbol_range pusch_symbols{2, 14};

  // Prioritize PCell over SCells.
  for (const ue_carrier* ue_cc : u.ue_carriers()) {
    const ul_harq_process* h = nullptr;
    if (is_retx) {
      h = ue_cc->harqs.find_pending_ul_retx();
    } else {
      h = ue_cc->harqs.find_empty_ul_harq();
    }
    if (h == nullptr) {
      continue;
    }
    unsigned                       k2   = 4; // TODO: Take from config.
    const cell_slot_resource_grid& grid = pusch_alloc.ul_resource_grid(ue_cc->cell_index, k2);
    for (const bwp_configuration* bwp : ue_cc->cfg().ul_bwps) {
      if (bwp == nullptr) {
        continue;
      }
      prb_bitmap   used_crbs     = grid.used_crbs(*bwp, pusch_symbols);
      crb_interval ue_grant_crbs = find_empty_interval_of_length(used_crbs, bwp->crbs.length(), 0);
      if (not ue_grant_crbs.empty()) {
        pusch_alloc.allocate_pusch(ue_pusch_grant{
            &u, ue_cc->cell_index, h, ue_grant_crbs, pusch_symbols, k2, to_search_space_id(1), aggregation_level::n4});
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scheduler_time_rr::dl_sched(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues)
{
  // Increment Round-robin counter to prioritize different UEs.
  rr_count++;

  // Start with retxs
  auto retx_ue_function = [&pdsch_alloc](const ue& u) { return alloc_dl_ue(u, pdsch_alloc, true); };
  if (round_robin_apply(ues, rr_count, retx_ue_function)) {
    return;
  }

  // Allocate newtxs
  auto newtx_ue_function = [&pdsch_alloc](const ue& u) { return alloc_dl_ue(u, pdsch_alloc, false); };
  if (round_robin_apply(ues, rr_count, newtx_ue_function)) {
    return;
  }
}

void scheduler_time_rr::ul_sched(ue_pusch_allocator& pusch_alloc, const ue_list& ues)
{
  // Increment Round-robin counter to prioritize different UEs.
  rr_count++;

  // Start with retxs
  auto retx_ue_function = [&pusch_alloc](const ue& u) { return alloc_ul_ue(u, pusch_alloc, true); };
  if (round_robin_apply(ues, rr_count, retx_ue_function)) {
    return;
  }

  // Allocate newtxs
  auto newtx_ue_function = [&pusch_alloc](const ue& u) { return alloc_ul_ue(u, pusch_alloc, false); };
  if (round_robin_apply(ues, rr_count, newtx_ue_function)) {
    return;
  }
}

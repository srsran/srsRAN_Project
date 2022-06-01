/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_cell_grid_allocator.h"

using namespace srsgnb;

ue_cell_grid_allocator::ue_cell_grid_allocator(ue_list& ues_, srslog::basic_logger& logger_) :
  ues(ues_), logger(logger_)
{}

void ue_cell_grid_allocator::add_cell(du_cell_index_t          cell_index,
                                      pdcch_scheduler&         pdcch_sched,
                                      cell_resource_allocator& cell_alloc)
{
  cells.emplace(cell_index, cell_t{cell_index, &pdcch_sched, &cell_alloc});
}

bool ue_cell_grid_allocator::allocate_pdsch(const ue_pdsch_grant& grant)
{
  srsran_sanity_check(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsran_sanity_check(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
  ue& u = ues[grant.user->ue_index];

  // Verify UE carrier is active.
  ue_carrier* ue_cc = u.find_cc(grant.cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active()) {
    logger.warning("SCHED: PDSCH allocation failed. Cause: The UE={} carrier with cell_index={} is inactive",
                   u.ue_index,
                   grant.cell_index);
    return false;
  }
  const ue_cell_configuration& ue_cell_cfg = ue_cc->cfg();
  subcarrier_spacing           scs         = ue_cc->cfg().dl_bwps[ue_cc->active_bwp_id()]->scs;

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = get_res_alloc(grant.cell_index)[0];
  cell_slot_resource_allocator& pdsch_alloc = get_res_alloc(grant.cell_index)[grant.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdcch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler output list");
    return false;
  }

  // Verify there is no RB collision.
  if (pdsch_alloc.dl_res_grid.collides(scs, grant.symbols, grant.crbs)) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler RB resource grid.");
    return false;
  }

  // Find a SearchSpace candidate.
  search_space_id ss_id = MAX_NOF_SEARCH_SPACES;
  for (const search_space_configuration& ss : ue_cell_cfg.get_dl_search_spaces(ue_cc->active_bwp_id())) {
    // TODO: Check if uses the same DCI format.
    ss_id = ss.id;
  }
  if (ss_id == MAX_NOF_SEARCH_SPACES) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_dl_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_dl_pdcch_ue(
              pdcch_alloc, u.crnti, ue_cc->cfg(), ue_cc->active_bwp_id(), ss_id, grant.aggr_lvl, grant.dci_fmt);
  if (pdcch == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{grant_info::channel::sch, scs, grant.symbols, grant.crbs});

  // Allocate PDSCH.
  pdsch_alloc.result.dl.ue_grants.emplace_back();
  dl_msg_alloc& msg = pdsch_alloc.result.dl.ue_grants.back();
  msg.crnti         = u.crnti;
  msg.tbs.emplace_back();
  msg.tbs.back().lc_lst.emplace_back();
  msg.tbs.back().lc_lst.back().lcid = lcid_t::LCID_SRB0;

  return true;
}

bool ue_cell_grid_allocator::allocate_pusch(const ue_pusch_grant& grant)
{
  return false;
}

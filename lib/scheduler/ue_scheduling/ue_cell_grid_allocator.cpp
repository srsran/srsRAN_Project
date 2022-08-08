/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_cell_grid_allocator.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"

using namespace srsgnb;

ue_cell_grid_allocator::ue_cell_grid_allocator(ue_list& ues_, srslog::basic_logger& logger_) :
  ues(ues_), logger(logger_)
{
}

void ue_cell_grid_allocator::add_cell(du_cell_index_t          cell_index,
                                      pdcch_scheduler&         pdcch_sched,
                                      cell_resource_allocator& cell_alloc)
{
  cells.emplace(cell_index, cell_t{cell_index, &pdcch_sched, &cell_alloc});
}

bool ue_cell_grid_allocator::allocate_pdsch(const ue_pdsch_grant& grant)
{
  srsgnb_sanity_check(ues.contains(grant.user->ue_index), "Invalid UE candidate index={}", grant.user->ue_index);
  srsgnb_sanity_check(has_cell(grant.cell_index), "Invalid UE candidate cell_index={}", grant.cell_index);
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
  const cell_configuration&    cell_cfg    = ue_cell_cfg.cell_cfg_common;
  subcarrier_spacing           scs         = ue_cell_cfg.dl_bwps[ue_cc->active_bwp_id()]->scs;

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
  const search_space_configuration* ss_cfg = nullptr;
  for (const search_space_configuration& ss : ue_cell_cfg.get_dl_search_spaces(ue_cc->active_bwp_id())) {
    if (search_space_supports_dl_dci_format(ss, grant.dci_fmt)) {
      ss_cfg = &ss;
      break;
    }
  }
  if (ss_cfg == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No valid SearchSpace found.");
    return false;
  }

  // Allocate PDCCH position.
  pdcch_dl_information* pdcch =
      get_pdcch_sched(grant.cell_index)
          .alloc_dl_pdcch_ue(
              pdcch_alloc, u.crnti, ue_cc->cfg(), ue_cc->active_bwp_id(), ss_cfg->id, grant.aggr_lvl, grant.dci_fmt);
  if (pdcch == nullptr) {
    logger.warning("Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{grant_info::channel::sch, scs, grant.symbols, grant.crbs});

  // Allocate UE DL HARQ.
  uint8_t          time_resource = 0; // TODO.
  prb_interval     prbs          = crb_to_prb(*pdcch->ctx.bwp_cfg, grant.crbs);
  dl_harq_process& h_dl          = ue_cc->harqs.dl_harq(grant.h_id);
  slot_point       uci_slot      = pdsch_alloc.slot + 4; // TODO.
  if (h_dl.empty()) {
    // It is a new tx.
    const static unsigned mcs      = 10; // TODO.
    const static unsigned max_retx = 4;  // TODO.
    bool                  success  = h_dl.new_tx(pdsch_alloc.slot, uci_slot, prbs, mcs, max_retx);
    srsgnb_assert(success, "Failed to allocate DL HARQ newtx");
  } else {
    // It is a retx.
    bool success = h_dl.new_retx(pdsch_alloc.slot, uci_slot, prbs);
    srsgnb_assert(success, "Failed to allocate DL HARQ retx");
  }

  // Fill DL PDCCH DCI.
  // TODO.

  // Fill PDSCH.
  pdsch_alloc.result.dl.ue_grants.emplace_back();
  dl_msg_alloc& msg     = pdsch_alloc.result.dl.ue_grants.back();
  msg.pdsch_cfg.rnti    = u.crnti;
  msg.pdsch_cfg.bwp_cfg = pdcch->ctx.bwp_cfg;
  msg.pdsch_cfg.prbs    = crb_to_prb(*msg.pdsch_cfg.bwp_cfg, grant.crbs);
  msg.pdsch_cfg.symbols = grant.symbols;
  // TODO: Use UE-dedicated DMRS info.
  msg.pdsch_cfg.dmrs = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  // See TS 38.211, 7.3.1.1. - Scrambling.
  msg.pdsch_cfg.n_id = cell_cfg.pci; // TODO.
  // Add codeword.
  msg.pdsch_cfg.codewords.emplace_back();
  pdsch_codeword&                          cw     = msg.pdsch_cfg.codewords.back();
  static constexpr std::array<unsigned, 4> rv_idx = {0, 2, 3, 1};
  cw.rv_index                                     = rv_idx[h_dl.nof_retx() % rv_idx.size()];
  cw.mcs_index                                    = h_dl.mcs(0);
  cw.mcs_table                                    = pdsch_mcs_table::qam64;
  sch_mcs_description mcs_config                  = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.qam_mod                                      = mcs_config.modulation;
  cw.target_code_rate                             = mcs_config.target_code_rate;
  // Set MAC logical channels to schedule in this PDU.
  msg.tb_list.emplace_back();
  msg.tb_list.back().lc_lst.emplace_back();
  msg.tb_list.back().lc_lst.back().lcid = lcid_t::LCID_SRB0;

  return true;
}

bool ue_cell_grid_allocator::allocate_pusch(const ue_pusch_grant& grant)
{
  return false;
}

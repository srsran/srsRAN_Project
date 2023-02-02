/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_resource_allocator_impl.h"
#include "../support/config_helpers.h"
#include "../support/pdcch/pdcch_pdu_parameters.h"
#include "pdcch_config_helpers.h"
#include "srsgnb/ran/pdcch/cce_to_prb_mapping.h"
#include "srsgnb/ran/pdcch/pdcch_candidates.h"

using namespace srsgnb;

class pdcch_resource_allocator_impl::pdcch_slot_allocator
{
public:
  /// PDCCH grant allocation in a given slot.
  struct alloc_record {
    bool                              is_dl;
    dci_context_information*          pdcch_ctx;
    const search_space_configuration* ss_cfg;
  };

  /// DFS decision tree node.
  struct tree_node {
    unsigned dci_iter_index;
    unsigned ncce;
    unsigned record_index;
  };

  explicit pdcch_slot_allocator(const cell_configuration& cell_cfg_, unsigned slot_index);
  ~pdcch_slot_allocator() = default;

  void clear();

  bool alloc_pdcch(dci_context_information&          pdcch_ctx,
                   cell_slot_resource_allocator&     slot_alloc,
                   const search_space_configuration& ss_cfg);

private:
  bool alloc_dfs_node(cell_slot_resource_allocator& slot_alloc, const alloc_record& record, unsigned dci_iter_index);
  bool get_next_dfs(cell_slot_resource_allocator& slot_alloc);

  pdcch_candidate_list get_cce_loc_table(slot_point pdcch_slot, const alloc_record& record) const;

  /// Allocate CCEs of a given PDCCH.
  bool allocate_cce(cell_slot_resource_allocator& slot_alloc, unsigned ncce, const alloc_record& record);

  const cell_configuration& cell_cfg;
  unsigned                  slot_index;

  /// list of grants in a given slot.
  static_vector<alloc_record, MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT> records;

  /// dfs decision tree for the given slot.
  std::vector<tree_node> dfs_tree, saved_dfs_tree;
};

pdcch_resource_allocator_impl::pdcch_slot_allocator::pdcch_slot_allocator(const cell_configuration& cell_cfg_,
                                                                          unsigned                  slot_index_) :
  cell_cfg(cell_cfg_), slot_index(slot_index_)
{
  (void)slot_index;
}

void pdcch_resource_allocator_impl::pdcch_slot_allocator::clear()
{
  records.clear();
  dfs_tree.clear();
  saved_dfs_tree.clear();
}

bool pdcch_resource_allocator_impl::pdcch_slot_allocator::alloc_pdcch(dci_context_information&          pdcch_ctx,
                                                                      cell_slot_resource_allocator&     slot_alloc,
                                                                      const search_space_configuration& ss_cfg)
{
  saved_dfs_tree.clear();

  // Create an DL Allocation Record.
  alloc_record record{};
  record.is_dl     = true;
  record.pdcch_ctx = &pdcch_ctx;
  record.ss_cfg    = &ss_cfg;

  // Try to allocate PDCCH for one of the possible CCE positions. If this operation fails, retry it, but using a
  // different permutation of past grant CCE positions.
  do {
    if (alloc_dfs_node(slot_alloc, record, 0)) {
      // Set CCE start index.
      record.pdcch_ctx->cces.ncce = dfs_tree.back().ncce;
      // DCI record was successfully allocated.
      records.push_back(record);
      // If the previous branches of the DFS changed in order for this new PDCCH to be allocated, update previous
      // PDCCH ncces
      if (not saved_dfs_tree.empty()) {
        for (unsigned i = 0; i != dfs_tree.size() - 1; ++i) {
          records[dfs_tree[i].record_index].pdcch_ctx->cces.ncce = dfs_tree[i].ncce;
        }
      }
      return true;
    }
    if (saved_dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs(slot_alloc));

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree.swap(saved_dfs_tree);

  return false;
}

bool pdcch_resource_allocator_impl::pdcch_slot_allocator::alloc_dfs_node(cell_slot_resource_allocator& slot_alloc,
                                                                         const alloc_record&           record,
                                                                         unsigned                      dci_iter_index)
{
  // Get CCE location table, i.e. the current node possible leaves.
  auto cce_locs = get_cce_loc_table(slot_alloc.slot, record);
  if (dci_iter_index >= cce_locs.size()) {
    // All possible CCE position leaves have been attempted. Early return.
    return false;
  }

  // Create new tree leave.
  tree_node node{};
  node.dci_iter_index = dci_iter_index;
  node.record_index   = dfs_tree.size();

  // Find in the list of possible CCEs, a CCE that does not cause PDCCH collisions.
  for (; node.dci_iter_index < cce_locs.size(); ++node.dci_iter_index) {
    node.ncce = cce_locs[node.dci_iter_index];

    // Attempt to allocate PDCCH with provided CCE position.
    if (not allocate_cce(slot_alloc, node.ncce, record)) {
      continue;
    }

    // Allocation successful. Add node to tree.
    dfs_tree.push_back(node);
    return true;
  }
  return false;
}

bool pdcch_resource_allocator_impl::pdcch_slot_allocator::get_next_dfs(cell_slot_resource_allocator& slot_alloc)
{
  do {
    if (dfs_tree.empty()) {
      // If we reach root, the allocation failed.
      return false;
    }
    // Attempt to re-add last tree node, but with a higher leaf index.
    uint32_t start_child_idx = dfs_tree.back().dci_iter_index + 1;
    dfs_tree.pop_back();
    while (dfs_tree.size() < records.size() and alloc_dfs_node(slot_alloc, records[dfs_tree.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (dfs_tree.size() < records.size());

  // Finished computation of next DFS node.
  return true;
}

pdcch_candidate_list
pdcch_resource_allocator_impl::pdcch_slot_allocator::get_cce_loc_table(slot_point          pdcch_slot,
                                                                       const alloc_record& record) const
{
  // TODO: Cache result list.
  aggregation_level l                = record.pdcch_ctx->cces.aggr_lvl;
  unsigned          nof_candidates   = record.ss_cfg->nof_candidates[to_aggregation_level_index(l)];
  unsigned          nof_coreset_cces = record.pdcch_ctx->coreset_cfg->get_nof_cces();
  if (record.ss_cfg->type == search_space_configuration::type_t::common) {
    return pdcch_candidates_common_ss_get_lowest_cce(
        pdcch_candidates_common_ss_configuration{l, nof_candidates, nof_coreset_cces});
  }
  return pdcch_candidates_ue_ss_get_lowest_cce(pdcch_candidates_ue_ss_configuration{l,
                                                                                    nof_candidates,
                                                                                    nof_coreset_cces,
                                                                                    record.pdcch_ctx->coreset_cfg->id,
                                                                                    record.pdcch_ctx->rnti,
                                                                                    pdcch_slot.slot_index()});
}

bool pdcch_resource_allocator_impl::pdcch_slot_allocator::allocate_cce(cell_slot_resource_allocator& slot_alloc,
                                                                       unsigned                      ncce,
                                                                       const alloc_record&           record)
{
  const bwp_configuration&     bwp_cfg = *record.pdcch_ctx->bwp_cfg;
  const coreset_configuration& cs_cfg  = *record.pdcch_ctx->coreset_cfg;
  prb_index_list pdcch_prbs = cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.pci, record.pdcch_ctx->cces.aggr_lvl, ncce);
  grant_info     grant;
  grant.scs = bwp_cfg.scs;

  // Check the current CCE position collides with an existing one.
  // TODO: Optimize.
  for (uint16_t prb : pdcch_prbs) {
    unsigned crb  = prb_to_crb(*record.pdcch_ctx->bwp_cfg, prb);
    grant.crbs    = {crb, crb + 1};
    grant.symbols = {0, (uint8_t)cs_cfg.duration};
    if (slot_alloc.dl_res_grid.collides(grant)) {
      // Collision detected. Try another CCE position.
      return false;
    }
  }

  // Allocation successful.
  // TODO: Optimize.
  for (uint16_t prb : pdcch_prbs) {
    unsigned crb  = prb_to_crb(*record.pdcch_ctx->bwp_cfg, prb);
    grant.crbs    = {crb, crb + 1};
    grant.symbols = {0, (uint8_t)cs_cfg.duration};
    slot_alloc.dl_res_grid.fill(grant);
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pdcch_resource_allocator_impl::pdcch_resource_allocator_impl(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_)
{
  subcarrier_spacing max_scs             = cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().scs;
  unsigned           nof_slots_per_frame = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(max_scs);
  for (unsigned i = 0; i < SLOT_ALLOCATOR_RING_SIZE; ++i) {
    unsigned slot_index = i % nof_slots_per_frame;
    slot_records[i]     = std::make_unique<pdcch_slot_allocator>(cell_cfg, slot_index);
  }
}

pdcch_resource_allocator_impl::~pdcch_resource_allocator_impl() = default;

void pdcch_resource_allocator_impl::slot_indication(slot_point sl_tx)
{
  srsgnb_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  // Clear old records.
  slot_records[(last_sl_ind - 1).to_uint() % slot_records.size()]->clear();
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                        rnti_t                        rnti,
                                                                        search_space_id               ss_id,
                                                                        aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[(size_t)ss_id];
  const coreset_configuration* cs_cfg = nullptr;
  if (ss_cfg.cs_id == to_coreset_id(0)) {
    cs_cfg = &(*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  } else {
    cs_cfg = &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();
  }

  return alloc_dl_pdcch_helper(slot_alloc, rnti, bwp_cfg, *cs_cfg, ss_cfg, aggr_lvl);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                           rnti_t                        rnti,
                                                                           search_space_id               ss_id,
                                                                           aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[(size_t)ss_id];
  const coreset_configuration* cs_cfg = (ss_cfg.cs_id == to_coreset_id(0))
                                            ? &(*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0)
                                            : &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();

  return alloc_ul_pdcch_helper(slot_alloc, rnti, bwp_cfg, *cs_cfg, ss_cfg, aggr_lvl);
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       bwp_id_t                      bwpid,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = user.dl_bwp_common(bwpid).generic_params;
  const search_space_configuration& ss_cfg  = user.search_space(ss_id);
  const coreset_configuration&      cs_cfg  = user.coreset(ss_cfg.cs_id);

  return alloc_dl_pdcch_helper(slot_alloc, rnti, bwp_cfg, cs_cfg, ss_cfg, aggr_lvl);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       bwp_id_t                      bwpid,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = user.ul_bwp_common(bwpid).generic_params;
  const search_space_configuration& ss_cfg  = user.search_space(ss_id);
  const coreset_configuration&      cs_cfg  = user.coreset(ss_cfg.cs_id);

  return alloc_ul_pdcch_helper(slot_alloc, rnti, bwp_cfg, cs_cfg, ss_cfg, aggr_lvl);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const coreset_configuration&      cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl)
{
  // Verify RNTI is unique.
  for (const pdcch_ul_information& pdcch : slot_alloc.result.dl.ul_pdcchs) {
    if (pdcch.ctx.rnti == rnti) {
      return nullptr;
    }
  }

  // Create PDCCH list element.
  pdcch_ul_information& pdcch = slot_alloc.result.dl.ul_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg;
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0; // Note: Value is set during the PDCCH allocation step.
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg, cs_cfg, ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg, ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg, cs_cfg);

  // Allocate a position for UL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, ss_cfg)) {
    slot_alloc.result.dl.ul_pdcchs.pop_back();
    return nullptr;
  }
  return &pdcch;
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const coreset_configuration&      cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl)
{
  if (not is_pdcch_monitoring_active(slot_alloc.slot, ss_cfg)) {
    // PDCCH monitoring is not active in this slot.
    return nullptr;
  }

  // Verify RNTI is unique.
  for (const pdcch_dl_information& pdcch : slot_alloc.result.dl.dl_pdcchs) {
    if (pdcch.ctx.rnti == rnti) {
      return nullptr;
    }
  }

  // Create PDCCH list element.
  pdcch_dl_information& pdcch = slot_alloc.result.dl.dl_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg;
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0;
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg, cs_cfg, ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg, ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg, cs_cfg);

  // Allocate a position for DL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, ss_cfg)) {
    slot_alloc.result.dl.dl_pdcchs.pop_back();
    return nullptr;
  }
  return &pdcch;
}

pdcch_resource_allocator_impl::pdcch_slot_allocator& pdcch_resource_allocator_impl::get_pdcch_slot_alloc(slot_point sl)
{
  srsgnb_sanity_check(sl < last_sl_ind + SLOT_ALLOCATOR_RING_SIZE, "PDCCH being allocated to far into the future");
  return *slot_records[sl.to_uint() % slot_records.size()];
}


#include "pdcch_scheduler.h"
#include "symbol_prb_res_grid.h"

using namespace srsgnb;

class pdcch_slot_allocator
{
public:
  /// PDCCH grant allocation in a given slot.
  struct alloc_record {
    bool               is_dl;
    pdcch_information* pdcch;
  };

  /// DFS decision tree node.
  struct tree_node {
    unsigned              dci_iter_index;
    unsigned              ncce;
    aggregation_level     L;
    symbol_x_prb_res_grid total_mask;

    explicit tree_node(const symbol_x_prb_res_grid& total_mask_) : total_mask(total_mask_) {}
  };

  pdcch_slot_allocator(unsigned nof_prbs_, bool cp_extended_) : nof_prbs(nof_prbs_), cp_extended(cp_extended_) {}

  void clear();

  bool alloc_pdcch(pdcch_information& pdcch, rnti_t rnti, aggregation_level L);

private:
  bool alloc_dfs_node(const alloc_record& record, unsigned dci_iter_index);
  bool get_next_dfs();

  // TODO
  span<unsigned> get_cce_loc_table(const alloc_record& record) { return {}; }

  unsigned nof_prbs;
  bool     cp_extended;

  /// list of grants in a given slot.
  static_vector<alloc_record, MAX_GRANTS> records;

  /// dfs decision tree for the given slot.
  std::vector<tree_node> dfs_tree, saved_dfs_tree;
};

class pdcch_scheduler::alloc_record_list
{
public:
  alloc_record_list(unsigned nof_prbs, bool cp_extended)
  {
    for (size_t i = 0; i < cell_resource_grid::RESOURCE_GRID_SIZE; ++i) {
      slots.emplace_back(nof_prbs, cp_extended);
    }
  }

  pdcch_slot_allocator& operator[](slot_point sl_tx)
  {
    return slots[sl_tx.to_uint() % cell_resource_grid::RESOURCE_GRID_SIZE];
  }

private:
  static_vector<pdcch_slot_allocator, cell_resource_grid::RESOURCE_GRID_SIZE> slots;
};

void pdcch_slot_allocator::clear()
{
  records.clear();
  dfs_tree.clear();
  saved_dfs_tree.clear();
}

bool pdcch_slot_allocator::alloc_pdcch(pdcch_information& pdcch, rnti_t rnti, aggregation_level L)
{
  saved_dfs_tree.clear();

  // Create an DL Allocation Record.
  alloc_record record{};
  record.is_dl           = true;
  record.pdcch           = &pdcch;
  record.pdcch->dci.rnti = rnti;
  record.pdcch->dci.L    = L;

  // Try to allocate grant. If it fails, attempt the same grant, but using a different permutation of past grant DCI
  // positions
  do {
    if (alloc_dfs_node(record, 0)) {
      // DCI record was successfully allocated.
      records.push_back(record);
      return true;
    }
    if (saved_dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs());

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree.swap(saved_dfs_tree);
  return false;
}

bool pdcch_slot_allocator::alloc_dfs_node(const alloc_record& record, unsigned dci_iter_index)
{
  // Get CCE location table, i.e. the current node leaves.
  auto cce_locs = get_cce_loc_table(record);
  if (dci_iter_index >= cce_locs.size()) {
    // All possible CCE position leaves have been attempted. Early return.
    return false;
  }

  // Create new tree leave.
  tree_node node{dfs_tree.empty() ? symbol_x_prb_res_grid{nof_prbs, cp_extended} : dfs_tree.back().total_mask};
  node.dci_iter_index = dci_iter_index;
  node.L              = record.pdcch->dci.L;

  for (; node.dci_iter_index < cce_locs.size(); ++node.dci_iter_index) {
    node.ncce = cce_locs[node.dci_iter_index];

    // Check the current PDCCH position collides with an existing one.
    if (node.total_mask.collides({}, {})) {
      // TODO
      // Collision detected. Try another CCE position.
      continue;
    }

    // Allocation successful.
    node.total_mask.fill({}, {});
    // TODO
    dfs_tree.push_back(node);
    return true;
  }
  return false;
}

bool pdcch_slot_allocator::get_next_dfs()
{
  do {
    if (dfs_tree.empty()) {
      // If we reach root, the allocation failed.
      return false;
    }
    // Attempt to re-add last tree node, but with a higher leaf index.
    uint32_t start_child_idx = dfs_tree.back().dci_iter_index + 1;
    dfs_tree.pop_back();
    while (dfs_tree.size() < records.size() and alloc_dfs_node(records[dfs_tree.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (dfs_tree.size() < records.size());

  // Finished computation of next DFS node
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pdcch_scheduler::pdcch_scheduler(cell_resource_grid& res_grid_) :
  res_grid(res_grid_), records(std::make_unique<alloc_record_list>(res_grid.cfg.nof_dl_prbs, false))
{}

pdcch_scheduler::~pdcch_scheduler() = default;

void pdcch_scheduler::slot_indication(slot_point sl_tx)
{
  last_sl_ind = sl_tx;

  // Clear old records.
  (*records)[last_sl_ind - 1].clear();
}

pdcch_information* pdcch_scheduler::alloc_pdcch_common(slot_point        sl_tx,
                                                       rnti_t            rnti,
                                                       search_space_id   ss_id,
                                                       aggregation_level L,
                                                       dci_format        dci_fmt)
{
  // Find Common BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = res_grid.cfg.dl_cfg_common.init_dl_bwp.generic_params;
  const search_space_configuration& ss_cfg =
      res_grid.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[(size_t)ss_id];
  const coreset_configuration& cs_cfg = res_grid.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coresets[ss_cfg.cs_id];

  return alloc_dl_pdcch_helper(sl_tx, rnti, bwp_cfg, cs_cfg, ss_cfg, L, dci_fmt);
}

pdcch_information* pdcch_scheduler::alloc_pdcch_ue(slot_point                      sl_tx,
                                                   rnti_t                          rnti,
                                                   const ue_carrier_configuration& user,
                                                   du_bwp_id_t                     bwp_id,
                                                   search_space_id                 ss_id,
                                                   aggregation_level               L,
                                                   dci_format                      dci_fmt)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = *user.get_bwp_cfg(bwp_id);
  const search_space_configuration& ss_cfg  = *user.get_ss_cfg(bwp_id, ss_id);
  const coreset_configuration&      cs_cfg  = *user.get_cs_cfg(bwp_id, ss_cfg.cs_id);

  return alloc_dl_pdcch_helper(sl_tx, rnti, bwp_cfg, cs_cfg, ss_cfg, L, dci_fmt);
}

pdcch_information* pdcch_scheduler::alloc_dl_pdcch_helper(slot_point                        sl_tx,
                                                          rnti_t                            rnti,
                                                          const bwp_configuration&          bwp_cfg,
                                                          const coreset_configuration&      cs_cfg,
                                                          const search_space_configuration& ss_cfg,
                                                          aggregation_level                 L,
                                                          dci_format                        dci_fmt)
{
  srsran_sanity_check(sl_tx - last_sl_ind < cell_resource_grid::MAXIMUM_SLOT_DIFF, "Invalid slot value");
  cell_slot_resource_grid& slot_res = res_grid[sl_tx];

  // Create PDCCH list element.
  slot_res.result.dl.dl_pdcchs.emplace_back();
  pdcch_information& pdcch = slot_res.result.dl.dl_pdcchs.back();

  // Set BWP and CORESET configurations.
  pdcch.bwp_cfg     = &bwp_cfg;
  pdcch.coreset_cfg = &cs_cfg;

  // Allocate DL PDCCH in CORESET.
  pdcch_slot_allocator& alloc = (*records)[sl_tx];
  if (not alloc.alloc_pdcch(pdcch, rnti, L)) {
    slot_res.result.dl.dl_pdcchs.pop_back();
    return nullptr;
  }
  return &pdcch;
}

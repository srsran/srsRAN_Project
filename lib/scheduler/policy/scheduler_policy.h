/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_allocator.h"

namespace srsgnb {

/// This struct provides a view of the current resource grid state to the PDSCH and PUSCH allocators.
class ue_resource_grid_view
{
public:
  void add_cell(const cell_resource_allocator& cell_grid)
  {
    cell_res_grids.emplace(cell_grid.cell_index(), &cell_grid);
  }

  slot_point get_pdcch_slot() const { return cell_res_grids[to_du_cell_index(0)]->slot_tx(); }

  const cell_configuration& get_cell_cfg_common(du_cell_index_t cell_index) const
  {
    return cell_res_grids[cell_index]->cfg;
  }

  const cell_slot_resource_grid& get_pdsch_grid(du_cell_index_t cell_index, unsigned k0) const
  {
    return (*cell_res_grids[cell_index])[k0].dl_res_grid;
  }

  const cell_slot_resource_grid& get_pusch_grid(du_cell_index_t cell_index, unsigned k2) const
  {
    return (*cell_res_grids[cell_index])[k2].ul_res_grid;
  }

  bool has_ue_dl_grant(du_cell_index_t cell_index, rnti_t rnti, unsigned k0) const
  {
    const auto& grants = (*cell_res_grids[cell_index])[k0].result.dl.ue_grants;
    return std::any_of(
        grants.begin(), grants.end(), [rnti](const auto& grant) { return grant.pdsch_cfg.rnti == rnti; });
  }

  bool has_ue_ul_grant(du_cell_index_t cell_index, rnti_t rnti, unsigned k2) const
  {
    const auto& puschs = (*cell_res_grids[cell_index])[k2].result.ul.puschs;
    return std::any_of(
        puschs.begin(), puschs.end(), [rnti](const auto& pusch) { return pusch.pusch_cfg.rnti == rnti; });
  }

  unsigned nof_cells() const { return cell_res_grids.size(); }

private:
  slotted_array<const cell_resource_allocator*, MAX_NOF_DU_CELLS> cell_res_grids;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  /// Schedule UE DL grants for a given slot and one or more cells.
  /// \param[out] pdsch_alloc PDSCH grant allocator. This object provides a handle to allocate PDSCH grants in the
  ///                            gNB resource grid.
  /// \param[in] res_grid view of the current resource grid occupancy state for all gnb cells.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  /// \param[in] is_retx Flag indicating DL grants for retransmissions or new transmissions.
  virtual void dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                        const ue_resource_grid_view& res_grid,
                        const ue_list&               ues,
                        bool                         is_retx) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  /// \param[out] pusch_alloc PUSCH grant allocator. This object provides a handle to allocate PUSCH grants in the
  ///                            gNB resource grid.
  /// \param[in] res_grid view of the current resource grid occupancy state for all gnb cells.
  /// \param[in] ues List of eligible UEs to be scheduled in the given slot.
  /// \param[in] is_retx Flag indicating UL grants for retransmissions or new transmissions.
  virtual void ul_sched(ue_pusch_allocator&          pusch_alloc,
                        const ue_resource_grid_view& res_grid,
                        const ue_list&               ues,
                        bool                         is_retx) = 0;
};

} // namespace srsgnb

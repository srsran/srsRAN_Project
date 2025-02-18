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

#include "../slicing/ran_slice_candidate.h"
#include "ue_allocator.h"

namespace srsran {

class ue_pdsch_grid_allocator
{
public:
  virtual ~ue_pdsch_grid_allocator() = default;

  virtual dl_alloc_result allocate_dl_grant(du_cell_index_t               cell_index,
                                            const dl_ran_slice_candidate& candidate,
                                            const ue_pdsch_grant&         grant) = 0;
};

/// Contextual information used by the scheduler policy to make decisions for a slot, cell and slice in DL.
class slice_dl_sched_context
{
public:
  /// Creates a new slice DL scheduling context.
  /// \param cell_grid_ Current resource grid occupancy state for given cell.
  /// \param slice_ Slice candidate to be scheduled in the given slot.
  /// \param allocator_ PDSCH grant allocator. This object provides a handle to allocate PDSCH grants in the gNB
  /// resource grid.
  /// \param harq_pending_retx_list_ List of DL HARQs pending retransmissions.
  slice_dl_sched_context(const cell_resource_allocator& cell_grid_,
                         dl_ran_slice_candidate&        slice_,
                         ue_pdsch_grid_allocator&       allocator_,
                         dl_harq_pending_retx_list      harq_pending_retx_list_) :
    cell_grid(cell_grid_),
    slice(slice_),
    allocator(allocator_),
    harq_pending_retx_list(harq_pending_retx_list_),
    k0(slice.get_slot_tx() - cell_grid.slot_tx())
  {
  }

  du_cell_index_t           cell_index() const { return cell_grid.cfg.cell_index; }
  const cell_configuration& get_cell_cfg_common() const { return cell_grid.cfg; }

  ran_slice_id_t                slice_id() const { return slice.id(); }
  const dl_ran_slice_candidate& get_slice() const { return slice; }

  slot_point pdcch_slot() const { return cell_grid.slot_tx(); }
  slot_point pdsch_slot() const { return cell_grid.slot_tx() + k0; }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() const { return cell_grid[0].result.dl.dl_pdcchs; }

  bool has_ue_dl_pdcch(rnti_t rnti) const
  {
    auto pdcchs = scheduled_dl_pdcchs();
    return std::any_of(
        pdcchs.begin(), pdcchs.end(), [rnti](const pdcch_dl_information& pdcch) { return pdcch.ctx.rnti == rnti; });
  }

  span<const dl_msg_alloc> scheduled_pdschs() const { return cell_grid[k0].result.dl.ue_grants; }

  bool has_ue_dl_grant(rnti_t rnti) const
  {
    auto pdschs = scheduled_pdschs();
    return std::any_of(
        pdschs.begin(), pdschs.end(), [rnti](const auto& grant) { return grant.pdsch_cfg.rnti == rnti; });
  }

  dl_alloc_result allocate_dl_grant(const ue_pdsch_grant& grant)
  {
    const dl_alloc_result result = allocator.allocate_dl_grant(cell_grid.cfg.cell_index, slice, grant);
    if (result.status == alloc_status::success) {
      slice.store_grant(result.alloc_nof_rbs);
    }
    return result;
  }

  /// Get a range view of DL HARQs pending retransmissions for the slice.
  auto get_harqs_with_pending_retx() { return harq_pending_retx_list; }

private:
  const cell_resource_allocator& cell_grid;
  dl_ran_slice_candidate&        slice;
  ue_pdsch_grid_allocator&       allocator;
  dl_harq_pending_retx_list      harq_pending_retx_list;
  const unsigned                 k0;
};

class ue_pusch_grid_allocator
{
public:
  virtual ~ue_pusch_grid_allocator() = default;

  virtual ul_alloc_result allocate_ul_grant(du_cell_index_t               cell_index,
                                            const ul_ran_slice_candidate& candidate,
                                            const ue_pusch_grant&         grant) = 0;
};

/// Contextual information used by the scheduler policy to make decisions for a slot, cell and slice in UL.
class slice_ul_sched_context
{
public:
  /// Creates a new slice UL scheduling context.
  /// \param cell_grid_ Current resource grid occupancy state for given cell.
  /// \param slice_ Slice candidate to be scheduled in the given slot.
  /// \param allocator_ PUSCH grant allocator. This object provides a handle to allocate PUSCH grants in the gNB
  /// resource grid.
  /// \param harq_pending_retx_list_ List of UL HARQs pending retransmissions.
  /// \param k2_ Delay between PDCCH and PUSCH slots.
  slice_ul_sched_context(const cell_resource_allocator& cell_grid_,
                         ul_ran_slice_candidate&        slice_,
                         ue_pusch_grid_allocator&       allocator_,
                         ul_harq_pending_retx_list      harq_pending_retx_list_) :
    cell_grid(cell_grid_),
    slice(slice_),
    allocator(allocator_),
    harq_pending_retx_list(harq_pending_retx_list_),
    k2(slice.get_slot_tx() - cell_grid.slot_tx())
  {
  }

  du_cell_index_t           cell_index() const { return cell_grid.cfg.cell_index; }
  const cell_configuration& get_cell_cfg_common() const { return cell_grid.cfg; }

  ran_slice_id_t                slice_id() const { return slice.id(); }
  const ul_ran_slice_candidate& get_slice() const { return slice; }

  slot_point pdcch_slot() const { return cell_grid.slot_tx(); }
  slot_point pusch_slot() const { return cell_grid.slot_tx() + k2; }

  span<const pdcch_ul_information> scheduled_ul_pdcchs() const { return cell_grid[0].result.dl.ul_pdcchs; }

  bool has_ue_ul_pdcch(rnti_t rnti) const
  {
    auto pdcchs = scheduled_ul_pdcchs();
    return std::any_of(
        pdcchs.begin(), pdcchs.end(), [rnti](const pdcch_ul_information& pdcch) { return pdcch.ctx.rnti == rnti; });
  }

  span<const ul_sched_info> scheduled_puschs() const { return cell_grid[k2].result.ul.puschs; }

  bool has_ue_ul_grant(rnti_t rnti) const
  {
    auto puschs = scheduled_puschs();
    return std::any_of(
        puschs.begin(), puschs.end(), [rnti](const auto& grant) { return grant.pusch_cfg.rnti == rnti; });
  }

  ul_alloc_result allocate_ul_grant(const ue_pusch_grant& grant)
  {
    const ul_alloc_result result = allocator.allocate_ul_grant(cell_grid.cfg.cell_index, slice, grant);
    if (result.status == alloc_status::success) {
      slice.store_grant(result.alloc_nof_rbs);
    }
    return result;
  }

  /// Get a range view of DL HARQs pending retransmissions for the slice.
  auto get_harqs_with_pending_retx() { return harq_pending_retx_list; }

private:
  const cell_resource_allocator& cell_grid;
  ul_ran_slice_candidate&        slice;
  ue_pusch_grid_allocator&       allocator;
  ul_harq_pending_retx_list      harq_pending_retx_list;
  const unsigned                 k2;
};

} // namespace srsran

/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

struct cell_slot_resource_allocator;
class cell_configuration;

class pdcch_slot_allocator
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

  explicit pdcch_slot_allocator(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_) {}
  ~pdcch_slot_allocator();

  /// Erase the current PDCCH allocations and stored context for this slot.
  void clear();

  /// \brief Searches for an available CCEs in the PDCCH to allocate a DCI. If it finds it, it reserves the CCEs, so
  /// they cannot be reused in this same slot.
  bool alloc_pdcch(dci_context_information&          pdcch_ctx,
                   cell_slot_resource_allocator&     slot_alloc,
                   const search_space_configuration& ss_cfg);

  /// Deallocates the last PDCCH CCE space reservation.
  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc);

private:
  bool alloc_dfs_node(cell_slot_resource_allocator& slot_alloc, const alloc_record& record, unsigned dci_iter_index);
  bool get_next_dfs(cell_slot_resource_allocator& slot_alloc);

  pdcch_candidate_list get_cce_loc_table(slot_point pdcch_slot, const alloc_record& record) const;

  /// Allocate CCEs of a given PDCCH.
  bool allocate_cce(cell_slot_resource_allocator& slot_alloc, unsigned ncce, const alloc_record& record);

  const cell_configuration& cell_cfg;

  /// list of grants in a given slot.
  static_vector<alloc_record, MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT> records;

  /// dfs decision tree for the given slot.
  std::vector<tree_node> dfs_tree, saved_dfs_tree;
};

} // namespace srsran
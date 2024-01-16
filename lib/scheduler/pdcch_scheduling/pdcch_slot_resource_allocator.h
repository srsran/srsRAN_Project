/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "../support/pdcch/pdcch_mapping.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

struct cell_slot_resource_allocator;
class cell_configuration;

/// List of CRBs for a given PDCCH candidate.
using crb_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

class pdcch_slot_allocator
{
public:
  /// PDCCH grant allocation in a given slot.
  struct alloc_record {
    bool                              is_dl;
    dci_context_information*          pdcch_ctx;
    const search_space_configuration* ss_cfg;
    span<const uint8_t>               pdcch_candidates;
    span<const crb_index_list>        pdcch_candidate_crbs;
  };

  /// DFS decision tree node.
  struct tree_node {
    unsigned dci_iter_index;
    unsigned ncce;
    unsigned record_index;
  };

  ~pdcch_slot_allocator();

  /// Erase the current PDCCH allocations and stored context for this slot.
  void clear();

  /// \brief Searches for an available CCEs in the PDCCH to allocate a DCI. If it finds it, it reserves the CCEs, so
  /// they cannot be reused in this same slot.
  bool alloc_pdcch(dci_context_information&          pdcch_ctx,
                   cell_slot_resource_allocator&     slot_alloc,
                   const search_space_configuration& ss_cfg,
                   span<const pdcch_candidate_type>  ss_candidates,
                   span<const crb_index_list>        ss_candidate_crbs);

  /// Deallocates the last PDCCH CCE space reservation.
  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc);

private:
  bool alloc_dfs_node(cell_slot_resource_allocator& slot_alloc, const alloc_record& record, unsigned dci_iter_index);
  bool get_next_dfs(cell_slot_resource_allocator& slot_alloc);

  /// Allocate CCEs of a given PDCCH.
  bool allocate_cce(cell_slot_resource_allocator& slot_alloc, const alloc_record& record, unsigned dci_iter_index);

  /// list of grants in a given slot.
  static_vector<alloc_record, MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT> records;

  /// dfs decision tree for the given slot.
  std::vector<tree_node> dfs_tree, saved_dfs_tree;
};

} // namespace srsran
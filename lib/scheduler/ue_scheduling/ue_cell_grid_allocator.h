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

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../policy/ue_allocator.h"
#include "../slicing/ran_slice_candidate.h"
#include "../uci_scheduling/uci_scheduler.h"
#include "ue_repository.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"

namespace srsran {

/// This class implements the ue_pdsch_allocator and ue_pusch_allocator interfaces, namely the methods to allocate
/// PDSCH and PUSCH grants in different cells, and the methods to fetch the current gNB resource grid DL and UL states.
class ue_cell_grid_allocator : public ue_pdsch_allocator, public ue_pusch_allocator
{
public:
  ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                         ue_repository&                    ues_,
                         srslog::basic_logger&             logger_);

  /// Adds a new cell to the UE allocator.
  void add_cell(du_cell_index_t           cell_index,
                pdcch_resource_allocator& pdcch_sched,
                uci_allocator&            uci_alloc,
                cell_resource_allocator&  cell_alloc);

  size_t nof_cells() const { return cells.size(); }

  void slot_indication(slot_point sl);

  alloc_result allocate_dl_grant(const ue_pdsch_grant& grant) override;

  alloc_result allocate_ul_grant(const ue_pusch_grant& grant) override;

private:
  struct cell_t {
    du_cell_index_t           cell_index;
    pdcch_resource_allocator* pdcch_sched;
    uci_allocator*            uci_alloc;
    cell_resource_allocator*  cell_alloc;
  };

  bool has_cell(du_cell_index_t cell_index) const { return cells.contains(cell_index); }

  pdcch_resource_allocator& get_pdcch_sched(du_cell_index_t cell_index) { return *cells[cell_index].pdcch_sched; }

  uci_allocator& get_uci_alloc(du_cell_index_t cell_index) { return *cells[cell_index].uci_alloc; }

  cell_resource_allocator&       get_res_alloc(du_cell_index_t cell_index) { return *cells[cell_index].cell_alloc; }
  const cell_resource_allocator& get_res_alloc(du_cell_index_t cell_index) const
  {
    return *cells[cell_index].cell_alloc;
  }

  const scheduler_ue_expert_config& expert_cfg;

  ue_repository&        ues;
  srslog::basic_logger& logger;

  slotted_array<cell_t, MAX_NOF_DU_CELLS> cells;

  // List of slots at which there is no PDSCH space for further allocations.
  static_vector<slot_point, SCHEDULER_MAX_K0> slots_with_no_pdsch_space;
  // List of slots at which there is no PUSCH space for further allocations.
  static_vector<slot_point, SCHEDULER_MAX_K2> slots_with_no_pusch_space;

  // Number of allocation attempts for DL and UL in the given slot.
  unsigned dl_attempts_count = 0, ul_attempts_count = 0;
};

/// This class implements the ue_pdsch_allocator interface and updates a DL slice candidate with the allocated RBs if
/// the PDSCH grant allocation is successful.
class dl_slice_ue_cell_grid_allocator : public ue_pdsch_allocator
{
public:
  dl_slice_ue_cell_grid_allocator(ue_pdsch_allocator& pdsch_alloc_, dl_ran_slice_candidate& slice_candidate_) :
    pdsch_alloc(pdsch_alloc_), slice_candidate(slice_candidate_)
  {
  }

  alloc_result allocate_dl_grant(const ue_pdsch_grant& grant) override
  {
    const alloc_result result = pdsch_alloc.allocate_dl_grant(grant);
    if (result.status == alloc_status::success) {
      slice_candidate.store_grant(result.alloc_nof_rbs);
    }
    return result;
  }

private:
  ue_pdsch_allocator&     pdsch_alloc;
  dl_ran_slice_candidate& slice_candidate;
};

/// This class implements the ue_pusch_allocator interface and updates a UL slice candidate with the allocated RBs if
/// the PUSCH grant allocation is successful.
class ul_slice_ue_cell_grid_allocator : public ue_pusch_allocator
{
public:
  ul_slice_ue_cell_grid_allocator(ue_pusch_allocator& pusch_alloc_, ul_ran_slice_candidate& slice_candidate_) :
    pusch_alloc(pusch_alloc_), slice_candidate(slice_candidate_)
  {
  }

  alloc_result allocate_ul_grant(const ue_pusch_grant& grant) override
  {
    const alloc_result result = pusch_alloc.allocate_ul_grant(grant);
    if (result.status == alloc_status::success) {
      slice_candidate.store_grant(result.alloc_nof_rbs);
    }
    return result;
  }

private:
  ue_pusch_allocator&     pusch_alloc;
  ul_ran_slice_candidate& slice_candidate;
};

} // namespace srsran

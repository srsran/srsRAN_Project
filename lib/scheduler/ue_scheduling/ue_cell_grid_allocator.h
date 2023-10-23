/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  void slot_indication();

  alloc_outcome allocate_dl_grant(const ue_pdsch_grant& grant) override;

  alloc_outcome allocate_ul_grant(const ue_pusch_grant& grant) override;

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

  // Number of allocation attempts for DL and UL in the given slot.
  unsigned dl_attempts_count = 0, ul_attempts_count = 0;
};

} // namespace srsran

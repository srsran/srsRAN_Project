/*
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../policy/ue_allocator.h"
#include "../uci_scheduling/uci_scheduler.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

namespace srsgnb {

/// This class implements the ue_pdsch_allocator and ue_pusch_allocator interfaces, namely the methods to allocate
/// PDSCH and PUSCH grants in different cells, and the methods to fetch the current gNB resource grid DL and UL states.
class ue_cell_grid_allocator : public ue_pdsch_allocator, public ue_pusch_allocator
{
public:
  ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_, ue_list& ues_, srslog::basic_logger& logger_);

  /// Adds a new cell to the UE allocator.
  void add_cell(du_cell_index_t           cell_index,
                pdcch_resource_allocator& pdcch_sched,
                uci_allocator&            uci_alloc,
                cell_resource_allocator&  cell_alloc);

  size_t nof_cells() const { return cells.size(); }

  bool allocate_dl_grant(const ue_pdsch_grant& grant) override;

  bool allocate_ul_grant(const ue_pusch_grant& grant) override;

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

  ue_list&              ues;
  srslog::basic_logger& logger;

  slotted_array<cell_t, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsgnb

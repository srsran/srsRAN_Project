/*
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../policy/slice_allocator.h"
#include "../policy/ue_allocator.h"
#include "../slicing/ran_slice_candidate.h"
#include "grant_params_selector.h"
#include "ue_repository.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"

namespace srsran {

/// This class implements the methods to allocate PDSCH and PUSCH grants in different cells for a slice, and the
/// methods to fetch the current gNB resource grid DL and UL states.
class ue_cell_grid_allocator final : public ue_pdsch_grid_allocator, public ue_pusch_grid_allocator
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

  dl_alloc_result allocate_dl_grant(du_cell_index_t               cell_index,
                                    const dl_ran_slice_candidate& slice,
                                    const ue_pdsch_grant&         grant) override;

  ul_alloc_result allocate_ul_grant(du_cell_index_t               cell_index,
                                    const ul_ran_slice_candidate& slice,
                                    const ue_pusch_grant&         grant) override;

  /// \brief Called at the end of a slot to process the allocations that took place and make some final adjustments.
  ///
  /// In particular, this function can redimension the existing grants to fill the remaining RBs if it deems necessary.
  void post_process_results();

private:
  struct cell_t {
    du_cell_index_t           cell_index;
    pdcch_resource_allocator* pdcch_sched;
    uci_allocator*            uci_alloc;
    cell_resource_allocator*  cell_alloc;
  };

  struct dl_grant_params {
    alloc_status                     status;
    search_space_id                  ss_id;
    uint8_t                          pdsch_td_res_index;
    crb_interval                     crb_lims;
    dci_dl_rnti_config_type          dci_type;
    const pdsch_config_params*       pdsch_cfg;
    sched_helper::mcs_prbs_selection recommended_mcs_prbs;
  };

  struct ul_grant_params {
    alloc_status                     status;
    search_space_id                  ss_id;
    uint8_t                          pusch_td_res_index;
    crb_interval                     crb_lims;
    dci_ul_rnti_config_type          dci_type;
    pusch_config_params              pusch_cfg;
    sched_helper::mcs_prbs_selection recommended_mcs_prbs;
  };

  dl_grant_params get_dl_grant_params(du_cell_index_t               cell_index,
                                      const dl_ran_slice_candidate& slice,
                                      const ue_pdsch_grant&         grant_params);

  ul_grant_params
  get_ul_grant_params(du_cell_index_t cell_index, const ul_ran_slice_candidate& slice, const ue_pusch_grant& grant);

  expected<pdcch_dl_information*, alloc_status> alloc_dl_pdcch(ue_cell& ue_cc, const search_space_info& ss_info);

  expected<uci_allocation, alloc_status>
  alloc_uci(ue_cell& ue_cc, const search_space_info& ss_info, uint8_t pdsch_td_res_index);

  bool has_cell(du_cell_index_t cell_index) const { return cells.contains(cell_index); }

  pdcch_resource_allocator& get_pdcch_sched(du_cell_index_t cell_index) { return *cells[cell_index].pdcch_sched; }

  uci_allocator& get_uci_alloc(du_cell_index_t cell_index) { return *cells[cell_index].uci_alloc; }

  cell_resource_allocator&       get_res_alloc(du_cell_index_t cell_index) { return *cells[cell_index].cell_alloc; }
  const cell_resource_allocator& get_res_alloc(du_cell_index_t cell_index) const
  {
    return *cells[cell_index].cell_alloc;
  }

  // Save the PUCCH power control results for the given slot.
  void post_process_pucch_pw_ctrl_results(du_cell_index_t cell_idx, slot_point slot);

  const scheduler_ue_expert_config& expert_cfg;

  ue_repository&        ues;
  srslog::basic_logger& logger;

  slotted_array<cell_t, MAX_NOF_DU_CELLS> cells;

  // List of slots at which there is no PDSCH space for further allocations.
  static_vector<slot_point, SCHEDULER_MAX_K0> slots_with_no_pdsch_space;

  // Number of allocation attempts for DL and UL in the given slot.
  unsigned dl_attempts_count = 0, ul_attempts_count = 0;
};

} // namespace srsran

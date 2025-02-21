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
#include "../slicing/ran_slice_candidate.h"
#include "../uci_scheduling/uci_allocator.h"
#include "grant_params_selector.h"
#include "ue_repository.h"

namespace srsran {

struct scheduler_ue_expert_config;

/// Information relative to a UE PDSCH grant.
struct ue_pdsch_grant {
  const slice_ue* user;
  harq_id_t       h_id;
  /// Recommended nof. bytes to schedule. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> recommended_nof_bytes;
  /// Maximum nof. RBs to allocate to the UE. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> max_nof_rbs;
};

/// Information relative to a UE PUSCH grant.
struct ue_pusch_grant {
  const slice_ue* user;
  harq_id_t       h_id;
  /// Recommended nof. bytes to schedule. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> recommended_nof_bytes;
  /// Maximum nof. RBs to allocate to the UE. This field is not present/ignored in case of HARQ retransmission.
  std::optional<unsigned> max_nof_rbs;
};

/// \brief Status of a UE grant allocation, and action for the scheduler policy to follow afterwards.
///
/// The current status are:
/// - success - the allocation was successful with the provided parameters.
/// - skip_slot - failure to allocate and the scheduler policy should terminate the current slot processing.
/// - skip_ue - failure to allocate and the scheduler policy should move on to the next candidate UE.
/// - invalid_params - failure to allocate and the scheduler policy should try a different set of grant parameters.
enum class alloc_status { success, skip_slot, skip_ue, invalid_params };

/// Allocation result of a UE DL grant allocation.
struct dl_alloc_result {
  alloc_status status;
  /// Nof. of bytes allocated if allocation was successful.
  unsigned alloc_bytes{0};
  /// Nof. of resource blocks allocated if allocation was successful.
  unsigned alloc_nof_rbs{0};
  /// List of logical channels scheduled in this TB if allocation was successful.
  dl_msg_tb_info tb_info;
};

/// Allocation result of a UE UL grant allocation.
struct ul_alloc_result {
  alloc_status status;
  /// Nof. of bytes allocated if allocation was successful.
  unsigned alloc_bytes{0};
  /// Nof. of resource blocks allocated if allocation was successful.
  unsigned alloc_nof_rbs{0};
};

/// This class implements the methods to allocate PDSCH and PUSCH grants in different cells for a slice, and the
/// methods to fetch the current gNB resource grid DL and UL states.
class ue_cell_grid_allocator
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

  dl_alloc_result
  allocate_dl_grant(du_cell_index_t cell_index, const dl_ran_slice_candidate& slice, const ue_pdsch_grant& grant);

  ul_alloc_result
  allocate_ul_grant(du_cell_index_t cell_index, const ul_ran_slice_candidate& slice, const ue_pusch_grant& grant);

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
};

} // namespace srsran

/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "../slicing/ran_slice_candidate.h"
#include "../uci_scheduling/uci_allocator.h"
#include "grant_params_selector.h"
#include "ue_repository.h"

namespace srsran {

struct scheduler_ue_expert_config;

/// Request for a newTx DL grant allocation.
struct ue_dl_newtx_grant_request {
  /// Slot at which PDSCH PDU shall be allocated.
  slot_point pdsch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// Pending bytes to schedule.
  unsigned pending_bytes;
  /// Maximum number of RBs to allocate. This limit can be determined based on slicing or UE configuration.
  std::optional<unsigned> max_nof_rbs;
};

/// Request for a reTx DL grant allocation.
struct ue_dl_retx_grant_request {
  /// Slot at which PDSCH PDU shall be allocated.
  slot_point pdsch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// HARQ process to be retransmitted.
  dl_harq_process_handle h_dl;
};

/// Request for a newTx UL grant allocation.
struct ue_ul_newtx_grant_request {
  /// Slot at which PUSCH PDU shall be allocated.
  slot_point pusch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// Pending bytes to schedule.
  unsigned pending_bytes;
  /// Maximum number of RBs to allocate. This limit can be determined based on slicing or UE configuration.
  std::optional<unsigned> max_nof_rbs;
};

/// Request for a reTx UL grant allocation.
struct ue_ul_retx_grant_request {
  /// Slot at which PUSCH PDU shall be allocated.
  slot_point pusch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// HARQ process to be retransmitted.
  ul_harq_process_handle h_ul;
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

/// \brief This class implements the methods to allocate PDCCH, UCI, PDSCH and PUSCH PDUs in the cell resource grid for
/// UE grants.
class ue_cell_grid_allocator
{
public:
  ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                         ue_repository&                    ues_,
                         pdcch_resource_allocator&         pdcch_sched_,
                         uci_allocator&                    uci_alloc_,
                         cell_resource_allocator&          cell_alloc_,
                         srslog::basic_logger&             logger_);

  /// Allocates DL grant for a UE newTx.
  dl_alloc_result allocate_newtx_dl_grant(const ue_dl_newtx_grant_request& request);

  /// Allocates DL grant for a UE reTx.
  dl_alloc_result allocate_retx_dl_grant(const ue_dl_retx_grant_request& request);

  /// Allocates UL grant for a UE newTx.
  ul_alloc_result allocate_newtx_ul_grant(const ue_ul_newtx_grant_request& request);

  /// Allocates UL grant for a UE reTx.
  ul_alloc_result allocate_retx_ul_grant(const ue_ul_retx_grant_request& request);

  /// \brief Called at the end of a slot to process the allocations that took place and make some final adjustments.
  ///
  /// In particular, this function can redimension the existing grants to fill the remaining RBs if it deems necessary.
  void post_process_results();

private:
  struct common_ue_dl_grant_request {
    slot_point                            pdsch_slot;
    const slice_ue*                       user;
    std::optional<dl_harq_process_handle> h_dl;
    std::optional<unsigned>               recommended_nof_bytes;
    std::optional<unsigned>               max_nof_rbs;
  };

  struct common_ue_ul_grant_request {
    slot_point                            pusch_slot;
    const slice_ue*                       user;
    std::optional<ul_harq_process_handle> h_ul;
    std::optional<unsigned>               recommended_nof_bytes;
    std::optional<unsigned>               max_nof_rbs;
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

  dl_alloc_result allocate_dl_grant(const common_ue_dl_grant_request& grant);

  ul_alloc_result allocate_ul_grant(const common_ue_ul_grant_request& grant);

  dl_grant_params get_dl_grant_params(const common_ue_dl_grant_request& grant_params);

  ul_grant_params get_ul_grant_params(const common_ue_ul_grant_request& grant);

  expected<pdcch_dl_information*, alloc_status> alloc_dl_pdcch(const ue_cell& ue_cc, const search_space_info& ss_info);

  expected<uci_allocation, alloc_status>
  alloc_uci(ue_cell& ue_cc, const search_space_info& ss_info, uint8_t pdsch_td_res_index);

  // Save the PUCCH power control results for the given slot.
  void post_process_pucch_pw_ctrl_results(slot_point slot);

  const scheduler_ue_expert_config& expert_cfg;
  ue_repository&                    ues;
  pdcch_resource_allocator&         pdcch_sched;
  uci_allocator&                    uci_alloc;
  cell_resource_allocator&          cell_alloc;
  srslog::basic_logger&             logger;
};

} // namespace srsran

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

#include "../policy/ue_allocator.h"
#include "../support/sch_pdu_builder.h"
#include "../support/slot_event_list.h"
#include "ue_repository.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include <queue>

namespace srsran {

/// Defines SRB0 scheduler that is used to allocate grants for UE's SRB0 DL messages in a given slot.
class ue_srb0_scheduler
{
public:
  explicit ue_srb0_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                             const cell_configuration&         cell_cfg_,
                             pdcch_resource_allocator&         pdcch_sch_,
                             pucch_allocator&                  pucch_alloc_,
                             ue_repository&                    ues_);

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  void handle_dl_buffer_state_indication(du_ue_index_t ue_index);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_srb0(cell_resource_allocator& res_alloc, ue& u);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  bool schedule_srb0(ue& u, cell_resource_allocator& res_alloc, unsigned pdsch_time_res);

  void fill_srb0_grant(ue&                        u,
                       slot_point                 pdsch_slot,
                       dl_harq_process&           h_dl,
                       pdcch_dl_information&      pdcch,
                       dl_msg_alloc&              msg,
                       unsigned                   pucch_res_indicator,
                       unsigned                   pdsch_time_res,
                       unsigned                   k1,
                       sch_mcs_index              mcs_idx,
                       const crb_interval&        ue_grant_crbs,
                       const pdsch_config_params& pdsch_params,
                       unsigned                   tbs_bytes);

  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  const scheduler_ue_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  pucch_allocator&                  pucch_alloc;
  ue_repository&                    ues;

  bwp_configuration initial_active_dl_bwp;
  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  search_space_configuration ss_cfg;
  coreset_configuration      cs_cfg;

  /// List of UE's DU Indexes for which SRB0 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ues;

  srslog::basic_logger& logger;
};

} // namespace srsran

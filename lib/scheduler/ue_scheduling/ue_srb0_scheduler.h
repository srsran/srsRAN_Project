/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../policy/ue_allocator.h"
#include "../support/slot_event_list.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "ue.h"
#include <queue>

namespace srsgnb {

/// Defines SRB0 scheduler that is used to allocate grants for UE's SRB0 DL messages in a given slot.
class ue_srb0_scheduler
{
public:
  explicit ue_srb0_scheduler(const cell_configuration& cell_cfg_,
                             pdcch_resource_allocator& pdcch_sch_,
                             pucch_scheduler&          pucch_sch_,
                             ue_list&                  ues_,
                             unsigned                  max_msg4_mcs_index_);

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
  bool schedule_srb0(ue&                               u,
                     cell_resource_allocator&          res_alloc,
                     unsigned                          pdsch_time_res,
                     const search_space_configuration& ss_cfg);

  void fill_srb0_grant(ue&                   u,
                       harq_process&         h_dl,
                       pdcch_dl_information& pdcch,
                       dl_msg_alloc&         msg,
                       pucch_harq_ack_grant& pucch,
                       unsigned              pdsch_time_res,
                       unsigned              k1,
                       const prb_interval&   ue_grant_prbs);


  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  const cell_configuration& cell_cfg;
  pdcch_resource_allocator& pdcch_sch;
  pucch_scheduler&          pucch_sch;
  ue_list&                  ues;

  /// Maximum MCS index that can be assigned when scheduling MSG4.
  unsigned max_msg4_mcs_index;

  bwp_configuration initial_active_dl_bwp;

  /// List of UE's DU Indexes for which SRB0 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb

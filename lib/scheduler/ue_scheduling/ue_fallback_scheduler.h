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
#include <list>
#include <queue>

namespace srsran {

/// Defines SRB0 scheduler that is used to allocate grants for UE's SRB0 DL messages in a given slot.
class ue_fallback_scheduler
{
public:
  explicit ue_fallback_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                 const cell_configuration&         cell_cfg_,
                                 pdcch_resource_allocator&         pdcch_sch_,
                                 pucch_allocator&                  pucch_alloc_,
                                 ue_repository&                    ues_);

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  void handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// Erase the UEs' HARQ processes that have been acked from the SRB scheduler cache.
  void update_ongoing_ue_retxs();

  // Holds the most recent slot with PDSCH for SRB0/SRB1 and the most recent slot with the corresponding PUCCH.
  struct most_recent_tx_slots {
    slot_point most_recent_tx_slot;
    slot_point most_recent_ack_slot;
  };

  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_srb(cell_resource_allocator&       res_alloc,
                    ue&                            u,
                    bool                           is_srb0,
                    dl_harq_process*               h_dl_retx,
                    optional<most_recent_tx_slots> most_recent_tx_ack_slots);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  dl_harq_process* schedule_srb0(ue&                      u,
                                 cell_resource_allocator& res_alloc,
                                 unsigned                 pdsch_time_res,
                                 unsigned                 slot_offset,
                                 slot_point               most_recent_ack_slot,
                                 dl_harq_process*         h_dl_retx);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  dl_harq_process* schedule_srb1(ue&                      u,
                                 cell_resource_allocator& res_alloc,
                                 unsigned                 pdsch_time_res,
                                 unsigned                 slot_offset,
                                 slot_point               most_recent_ack_slot,
                                 dl_harq_process*         h_dl_retx = nullptr);

  void fill_srb_grant(ue&                        u,
                      slot_point                 pdsch_slot,
                      dl_harq_process&           h_dl,
                      pdcch_dl_information&      pdcch,
                      dci_dl_rnti_config_type    dci_type,
                      dl_msg_alloc&              msg,
                      unsigned                   pucch_res_indicator,
                      unsigned                   pdsch_time_res,
                      unsigned                   k1,
                      sch_mcs_index              mcs_idx,
                      const crb_interval&        ue_grant_crbs,
                      const pdsch_config_params& pdsch_params,
                      unsigned                   tbs_bytes,
                      bool                       is_retx,
                      bool                       is_srb0);

  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  struct srb_ue {
    du_ue_index_t ue_index;
    bool          is_srb0;
  };

  /// List of UE's DU Indexes for which SRB0 and SRB1 messages needs to be scheduled.
  std::vector<srb_ue> pending_ues;

  /// Class that keeps track of the UEs' that are waiting for the SRB HARQ processes to be ACKed or retransmitted.
  class ack_and_retx_tracker
  {
  public:
    explicit ack_and_retx_tracker(du_ue_index_t ue_idx, dl_harq_process* h_dl_, bool is_srb0_, ue_repository& ues_) :
      ue_index(ue_idx), is_srb0(is_srb0_), h_dl(h_dl_)
    {
    }
    explicit ack_and_retx_tracker(const ack_and_retx_tracker& other) = default;

    bool match_ue_harq(du_ue_index_t ue_idx_, dl_harq_process* h_dl_) const
    {
      return ue_index == ue_idx_ and h_dl == h_dl_;
    }

    du_ue_index_t    ue_index;
    bool             is_srb0;
    dl_harq_process* h_dl;
  };

  void store_harq_tx(du_ue_index_t ue_index, dl_harq_process* h_dl, bool is_srb0);

  // If there are any pending SRB0 or SRB1 transmissions for the UE, the function returns the most recent slot with
  // PDSCH for SRB0/SRB1 and the most recent slot with the corresponding PUCCH.
  optional<most_recent_tx_slots> get_most_recent_slot_tx(du_ue_index_t ue_idx) const;

  const scheduler_ue_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  // TODO: Find proper values for these 2 parameters.
  // Set the max number of slots the scheduler can look ahead in the resource grid (with respect to the current slot) to
  // find PDSCH space for SRB0 or SRB1.
  const unsigned max_dl_slots_ahead_sched = 10U;
  // Set the max number of attempts the scheduler can do while running through the nested loops over the PDSCH time
  // allocation indices and the ahead slots for all UEs. This is to avoid excessive long iterations in case many UEs.
  // NOTE: max_sched_attempts = (max_dl_slots_ahead_sched + 1) * max_pdsch_time_res guarantees that at 1 UE will be
  // allocated in a TDD period of 10 slots.
  const unsigned max_sched_attempts = 22U;
  // Counter for the scheduling attempts per function call.
  unsigned                  sched_attempts_cnt = 0;
  pdcch_resource_allocator& pdcch_sch;
  pucch_allocator&          pucch_alloc;
  ue_repository&            ues;

  bwp_configuration initial_active_dl_bwp;
  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  search_space_configuration ss_cfg;
  coreset_configuration      cs_cfg;

  /// Cache the UEs that are waiting for SRB HARQ processes to be ACKed or retransmitted.
  std::vector<ack_and_retx_tracker> ongoing_ues_ack_retxs;

  std::vector<uint8_t> dci_1_0_k1_values;

  srslog::basic_logger& logger;
};

} // namespace srsran

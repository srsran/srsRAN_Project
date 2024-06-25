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

/// Defines the scheduler that is used to allocate grants for DL SRB0 and SRB1 messages and UL SRB1 messages for UE in
/// fallback state.
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
  /// \param[in] is_srb0 Defines whether the DL Buffer State Indication is for SRB0/SRB1.
  /// \param[in] sl Slot at which the DL Buffer State Indication was received.
  /// \param[in] srb_buffer_bytes Number of SRB bytes reported by Buffer State Indication.
  void
  handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0, slot_point sl, unsigned srb_buffer_bytes);

  /// Handle Contention Resolution indication sent by the MAC.
  /// \param[in] ue_index UE's DU Index for which Contention Resolution CE needs to be scheduled.
  void handle_conres_indication(du_ue_index_t ue_index);

  /// Handles UL Buffer State Report indication reported by UE.
  /// \param[in] ue_index UE's DU Index for which UL SRB1 message needs to be scheduled.
  /// \param[in] bsr_ind Buffer State Report indication message.
  void handle_ul_bsr_indication(du_ue_index_t ue_index, const ul_bsr_indication_message& bsr_ind);

  /// Handles SR indication reported by UE.
  /// \param[in] ue_index UE's DU Index for which UL SRB1 message needs to be scheduled.
  void handle_sr_indication(du_ue_index_t ue_index);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// Helper that schedules DL SRB0 and SRB1 retx. Returns false if the DL fallback schedule should stop the DL
  /// allocation, true otherwise.
  bool schedule_dl_retx(cell_resource_allocator& res_alloc);

  /// Helper that schedules new UL SRB1 tx.
  void schedule_ul_new_tx_and_retx(cell_resource_allocator& res_alloc);

  /// Helper that schedules new DL ConRes CE when Msg4 over SRB has not yet been received. Returns false if the DL
  /// fallback schedule should exit, true otherwise.
  /// \remark This function handles the following scenarios:
  ///     - Schedules ConRes CE only if ConRes indication is received from MAC but no buffer status update is received
  ///       for SRB0/SRB1.
  bool schedule_dl_conres_new_tx(cell_resource_allocator& res_alloc);

  /// Helper that schedules new DL SRB0 tx. Returns false if the DL fallback schedule should exit, true otherwise.
  /// \remark This function handles the following scenarios:
  ///     - Schedules SRB0 only (not empty) if ConRes CE has already sent.
  ///     - Schedules SRB0 (not empty) + ConRes CE (if pending) if there is enough space in PDSCH resource grid.
  ///     - Schedules ConRes CE only (if pending) if there is not enough space in PDSCH resource grid to fit SRB0 (not
  ///       empty) + ConRes CE.
  bool schedule_dl_new_tx_srb0(cell_resource_allocator& res_alloc);

  /// Helper that schedules new DL SRB1 tx.
  /// \remark This function handles the following scenarios:
  ///     - Schedules SRB1 (not empty) + ConRes CE (if pending).
  void schedule_dl_new_tx_srb1(cell_resource_allocator& res_alloc);

  /// Size of the ring buffer used to store the slots where the scheduler has found no PDCCH/PDSCH resources.
  static const size_t FALLBACK_SCHED_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1 + NTN_CELL_SPECIFIC_KOFFSET_MAX);

  /// Erase the UEs' HARQ processes that have been acked from the SRB scheduler cache.
  void slot_indication(slot_point sl);

  // Holds the most recent slot with PDSCH for SRB0/SRB1 and the most recent slot with the corresponding PUCCH.
  struct most_recent_tx_slots {
    slot_point most_recent_tx_slot;
    slot_point most_recent_ack_slot;
  };

  /// \remark srb_pending => Only ConRes was scheduled and Msg4 is yet to be scheduled.
  enum class dl_sched_outcome { success, next_ue, stop_dl_scheduling, srb_pending };

  /// \brief Tries to schedule DL SRB0/SRB1 message and/or ConRes CE only for a UE, iterating over several PDSCH slots
  /// ahead of the current reference slot.
  /// \remark If \c is_srb0 is empty, then only ConRes CE is scheduled.
  dl_sched_outcome schedule_dl_srb(cell_resource_allocator&            res_alloc,
                                   ue&                                 u,
                                   dl_harq_process*                    h_dl_retx,
                                   std::optional<most_recent_tx_slots> most_recent_tx_ack_slots = std::nullopt,
                                   std::optional<bool>                 is_srb0                  = std::nullopt);

  enum class ul_srb_sched_outcome { next_ue, next_slot, stop_ul_scheduling };

  /// \brief Tries to schedule UL SRB1 message for a UE iterating over the possible k2 values. Returns true if the
  /// scheduler should keep allocating the next UL UE, false if it should stop the UL allocation.
  ul_srb_sched_outcome schedule_ul_ue(cell_resource_allocator& res_alloc, ue& u, ul_harq_process* h_ul_retx);

  struct sched_srb_results {
    dl_harq_process* h_dl = nullptr;
    // This field represents whether SRB data was scheduled along with ConRes CE or only ConRes CE was scheduled.
    bool is_srb_data_pending = false;
    // This is only meaningful for SRB1, and represents the number of LCID-1 bytes (excluding any overhead) that have
    // been scheduled for transmission.
    unsigned nof_srb1_scheduled_bytes = 0;
  };

  /// \brief Tries to schedule DL ConRes CE for a UE and for a specific PDSCH slot.
  /// \remark This function handles the following scenarios:
  ///     - Schedules ConRes CE only if ConRes indication is received from MAC but no buffer status update is received
  ///       for SRB0/SRB1.
  sched_srb_results schedule_dl_conres_ce(ue&                      u,
                                          cell_resource_allocator& res_alloc,
                                          unsigned                 pdsch_time_res,
                                          unsigned                 slot_offset,
                                          slot_point               most_recent_ack_slot,
                                          dl_harq_process*         h_dl_retx);

  /// \brief Tries to schedule DL SRB0 message for a UE and for a specific PDSCH slot.
  /// \remark This function handles the following scenarios:
  ///     - Schedules SRB0 only (not empty) if ConRes CE has already sent.
  ///     - Schedules SRB0 (not empty) + ConRes CE (if pending) if there is enough space in PDSCH resource grid.
  ///     - Schedules ConRes CE only (if pending) if there is not enough space in PDSCH resource grid to fit SRB0 (not
  ///       empty) + ConRes CE.
  sched_srb_results schedule_dl_srb0(ue&                      u,
                                     cell_resource_allocator& res_alloc,
                                     unsigned                 pdsch_time_res,
                                     unsigned                 slot_offset,
                                     slot_point               most_recent_ack_slot,
                                     dl_harq_process*         h_dl_retx);

  /// \brief Tries to schedule DL SRB1 message for a UE and for a specific PDSCH slot.
  /// \remark This function handles the following scenarios:
  ///     - Schedules SRB1 (not empty) + ConRes CE (if pending).
  sched_srb_results schedule_dl_srb1(ue&                      u,
                                     slot_point               sched_ref_slot,
                                     cell_resource_allocator& res_alloc,
                                     unsigned                 pdsch_time_res,
                                     unsigned                 slot_offset,
                                     slot_point               most_recent_ack_slot,
                                     dl_harq_process*         h_dl_retx = nullptr);

  /// \brief Tries to schedule SRB1 message for a specific PUSCH time domain resource.
  ul_srb_sched_outcome schedule_ul_srb(ue&                                          u,
                                       cell_resource_allocator&                     res_alloc,
                                       unsigned                                     pusch_time_res,
                                       const pusch_time_domain_resource_allocation& pusch_td,
                                       ul_harq_process*                             h_ul_retx);

  unsigned fill_dl_srb_grant(ue&                        u,
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
                             std::optional<bool>        is_srb0 = std::nullopt);

  void fill_ul_srb_grant(ue&                        u,
                         slot_point                 pdcch_slot,
                         ul_harq_process&           h_ul,
                         pdcch_ul_information&      pdcch,
                         ul_sched_info&             msg,
                         unsigned                   pusch_time_res,
                         unsigned                   k2_offset,
                         sch_mcs_index              mcs_idx,
                         const crb_interval&        ue_grant_crbs,
                         const pusch_config_params& pusch_params,
                         unsigned                   tbs_bytes,
                         bool                       is_retx);

  /// Returns the PDSCH time domain resource allocation for a given PDSCH time resource index.
  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  // Returns the PDSCH time resource index that is suitable for a given PDSCH configuration.
  std::optional<unsigned> get_pdsch_time_res_idx(const pdsch_config_common& pdsch_cfg,
                                                 slot_point                 sl_tx,
                                                 const dl_harq_process*     h_dl_retx) const;

  /// Defines the information that is needed to track the DL UEs that are pending for new SRB0/SRB1/ConRes CE TX.
  struct fallback_ue {
    du_ue_index_t ue_index;
    // This field is empty if only ConRes indication is received from MAC and buffer status from upper layers for
    // SRB0/SRB1 is not yet received.
    std::optional<bool> is_srb0;
    // This field indicated whether ConRes CE pending to be sent or not.
    bool is_conres_pending;
    // Represents the number of LCID-1 bytes (excluding any overhead) that are pending for this UE.
    // This is only meaningful for SRB1 and gets updated every time an RLC buffer state update is received or when we
    // schedule a new PDSCH TX for this UE.
    unsigned pending_srb1_buffer_bytes = 0;
  };

  /// List of UE's DU Indexes for which SRB0 and SRB1 messages needs to be scheduled.
  std::vector<fallback_ue> pending_dl_ues_new_tx;

  /// List of UE's DU Indexes that are pending for new TX or RE-TX.
  std::vector<du_ue_index_t> pending_ul_ues;

  /// Class that keeps track of the UEs' that are waiting for the SRB HARQ processes to be ACKed or retransmitted.
  class ack_and_retx_tracker
  {
  public:
    explicit ack_and_retx_tracker(du_ue_index_t       ue_idx,
                                  dl_harq_process*    h_dl_,
                                  ue_repository&      ues_,
                                  unsigned            srb_payload_bytes_,
                                  std::optional<bool> is_srb0_ = std::nullopt) :
      ue_index(ue_idx), is_srb0(is_srb0_), h_dl(h_dl_), srb1_payload_bytes(srb_payload_bytes_)
    {
    }
    explicit ack_and_retx_tracker(const ack_and_retx_tracker& other) = default;

    bool match_ue_harq(du_ue_index_t ue_idx_, dl_harq_process* h_dl_) const
    {
      return ue_index == ue_idx_ and h_dl == h_dl_;
    }

    du_ue_index_t ue_index;
    // This field is empty if HARQ is used to schedule ConRes CE only.
    std::optional<bool> is_srb0;
    dl_harq_process*    h_dl;
    // Represents the number of LCID-1 bytes (excluding any overhead) that have been allocated for this TX.
    // This is only meaningful for SRB1,
    unsigned srb1_payload_bytes = 0;
  };

  void store_harq_tx(du_ue_index_t       ue_index,
                     dl_harq_process*    h_dl,
                     unsigned            srb_payload_bytes,
                     std::optional<bool> is_srb0 = std::nullopt);

  // If there are any pending SRB0, SRB1 transmissions or ConRes CE for the UE, the function returns the most recent
  // slot with PDSCH for SRB0/SRB1/ConRes CE allocation and the most recent slot with the corresponding PUCCH.
  std::optional<most_recent_tx_slots> get_most_recent_slot_tx(du_ue_index_t ue_idx) const;

  // Returns the total number of bytes pending for SRB1 for a given UE, including MAC CE and MAC subheaders.
  unsigned get_srb1_pending_tot_bytes(du_ue_index_t ue_idx) const;

  // Checks if there are bytes pending for SRB1 for a given UE (including MAC CE and MAC subheaders).
  unsigned has_pending_bytes_for_srb1(du_ue_index_t ue_idx) const;

  // \brief Updates the UE's SRB1 buffer state (only LCID 1 bytes, without any overhead) after the allocation of a HARQ
  // process for a new transmission.
  // \param[in] ue_idx UE's DU Index for which SRB1 buffer state needs to be updated.
  // \param[in] allocated_bytes Number of bytes (only LCID 1 bytes, without any overhead) allocated on the PDSCH.
  void update_srb1_buffer_state_after_alloc(du_ue_index_t ue_idx, unsigned allocated_bytes);

  // \brief Updates the UE's SRB1 buffer state (only LCID 1 bytes, without any overhead) after receiving the RLC buffer
  // state update from upper layers.
  //
  // This function updates the fallback scheduler internal SRB1 buffer state for a given UE by subtracting the bytes for
  // the new allocations (that have been already completed but not yet transmitted) from the RLC buffer state update
  // that was received.
  //
  // \param[in] ue_idx UE's DU Index for which SRB1 buffer state needs to be updated.
  // \param[in] sl Slot at which the Buffer State Update was received.
  // \param[in] buffer_status_report Number of pending LCID 1 bytes reported by the RLC buffer state update (only LCID 1
  // bytes, without any overhead).
  void update_srb1_buffer_after_rlc_bsu(du_ue_index_t ue_idx, slot_point sl, unsigned buffer_status_report);

  const scheduler_ue_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  // TODO: Find proper values for these 2 parameters.
  // Set the max number of slots the scheduler can look ahead in the resource grid (with respect to the current slot) to
  // find PDSCH space for SRB0 or SRB1.
  const unsigned max_dl_slots_ahead_sched = 4U;
  // Set the max number of attempts the scheduler can do while running through the nested loops over the PDSCH time
  // allocation indices and the ahead slots for all UEs. This is to avoid excessive long iterations in case many UEs.
  // NOTE: max_dl_sched_attempts = (max_dl_slots_ahead_sched + 1) * max_pdsch_time_res guarantees that at 1 UE will be
  // allocated in a TDD period of 10 slots.
  const unsigned max_dl_sched_attempts = 22U;
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

  /// Cache the DL UEs that are waiting for SRB HARQ processes to be ACKed or retransmitted.
  std::vector<ack_and_retx_tracker> ongoing_ues_ack_retxs;

  // Ring buffer of booleans that indicate whether PDSCH over a slot should be skipped due to lack of PDCCH/PDSCH
  // resources.
  std::array<bool, FALLBACK_SCHED_RING_BUFFER_SIZE> slots_with_no_pdxch_space;
  // Keeps track of the last slot_point used by \ref slots_with_no_pdxch_space.
  slot_point last_sl_ind;

  std::vector<uint8_t> dci_1_0_k1_values;

  srslog::basic_logger& logger;
};

} // namespace srsran

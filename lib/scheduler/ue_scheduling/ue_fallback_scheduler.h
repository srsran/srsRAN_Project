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

#include "../support/sch_pdu_builder.h"
#include "../ue_context/ue_repository.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"

namespace srsran {

class pdcch_resource_allocator;
class pucch_allocator;
class uci_allocator;
struct cell_resource_allocator;
struct pdcch_dl_information;
struct pdcch_ul_information;
struct uci_allocation;

/// Defines the scheduler that is used to allocate grants for DL SRB0 and SRB1 messages and UL SRB1 messages for UE in
/// fallback state.
class ue_fallback_scheduler
{
public:
  explicit ue_fallback_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                 const cell_configuration&         cell_cfg_,
                                 pdcch_resource_allocator&         pdcch_sch_,
                                 // TODO: Remove pucch_alloc and depend only on uci_alloc
                                 pucch_allocator& pucch_alloc_,
                                 uci_allocator&   uci_alloc_,
                                 ue_repository&   ues_);

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  /// \param[in] is_srb0 Defines whether the DL Buffer State Indication is for SRB0/SRB1.
  void handle_dl_buffer_state_indication(du_ue_index_t ue_index);

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

  /// Called when cell is deactivated.
  void stop();

private:
  enum class dl_new_tx_alloc_type { conres_only, srb0, srb1, skip, error };

  /// Size of the ring buffer used to store the slots where the scheduler has found no PDCCH/PDSCH resources.
  static const size_t FALLBACK_SCHED_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1 + NTN_CELL_SPECIFIC_KOFFSET_MAX);

  /// Defines the information that is needed to track the DL UEs that are pending for new SRB0/SRB1/ConRes CE TX.
  struct fallback_ue {
    du_ue_index_t ue_index;
  };

  /// Helper that schedules DL SRB0 and SRB1 retx. Returns false if the DL fallback schedule should stop the DL
  /// allocation, true otherwise.
  bool schedule_dl_retx(cell_resource_allocator& res_alloc);

  /// Helper that schedules new UL SRB1 tx.
  void schedule_ul_new_tx_and_retx(cell_resource_allocator& res_alloc);

  /// Determine which type of DL allocation is required for a given UE candidate.
  static dl_new_tx_alloc_type get_dl_new_tx_alloc_type(const ue& u);

  /// Helper that schedules pending DL new txs for ConRes CE, SRB0 or SRB1.
  /// \param[in] selected_alloc_type Type of allocation to make. Options: ConRes CE only, SRB0 and SRB1.
  /// \return false if the fallback scheduler should stop its operation for the given slot.
  bool schedule_dl_new_tx(cell_resource_allocator& res_alloc, dl_new_tx_alloc_type selected_alloc_type);

  /// Erase the UEs' HARQ processes that have been acked from the SRB scheduler cache.
  void slot_indication(slot_point sl);

  /// \remark srb_pending => Only ConRes was scheduled and Msg4 is yet to be scheduled.
  enum class dl_sched_outcome { success, next_ue, stop_dl_scheduling };

  /// \brief Tries to schedule DL SRB0/SRB1 message and/or ConRes CE only for a UE, iterating over several PDSCH slots
  /// ahead of the current reference slot.
  /// \remark If \c is_srb0 is empty, then only ConRes CE is scheduled.
  dl_sched_outcome
  schedule_dl_srb(cell_resource_allocator& res_alloc, ue& u, std::optional<dl_harq_process_handle> h_dl_retx);

  enum class ul_srb_sched_outcome { next_ue, next_slot, stop_ul_scheduling };

  /// \brief Tries to schedule UL SRB1 message for a UE iterating over the possible k2 values. Returns true if the
  /// scheduler should keep allocating the next UL UE, false if it should stop the UL allocation.
  ul_srb_sched_outcome schedule_ul_ue(cell_resource_allocator& res_alloc, ue& u);

  /// \brief For a number of pending bytes, select the appropriate MCS.
  std::tuple<unsigned, sch_mcs_index, units::bytes> select_tbs(const pdsch_config_params&          pdsch_cfg,
                                                               unsigned                            pending_bytes,
                                                               const crb_interval&                 unused_crbs,
                                                               const std::optional<sch_mcs_index>& fixed_mcs) const;

  /// \brief Allocate DL grant for a UE and for a specific PDSCH slot.
  std::optional<dl_harq_process_handle> alloc_grant(ue&                                   u,
                                                    cell_resource_allocator&              res_alloc,
                                                    unsigned                              pdsch_time_res,
                                                    unsigned                              slot_offset,
                                                    slot_point                            most_recent_ack_slot,
                                                    std::optional<dl_harq_process_handle> h_dl_retx);

  /// \brief Tries to schedule SRB1 message for a specific PUSCH time domain resource.
  ul_srb_sched_outcome schedule_ul_srb(ue&                                          u,
                                       cell_resource_allocator&                     res_alloc,
                                       unsigned                                     pusch_time_res,
                                       const pusch_time_domain_resource_allocation& pusch_td,
                                       std::optional<ul_harq_process_handle>        h_ul_retx,
                                       bool                                         remove_pucch);

  /// \return A pair with the number of SRB bytes allocated and which DL HARQ process was used.
  dl_harq_process_handle fill_dl_srb_grant(ue&                                   u,
                                           slot_point                            pdsch_slot,
                                           std::optional<dl_harq_process_handle> h_dl,
                                           pdcch_dl_information&                 pdcch,
                                           dci_dl_rnti_config_type               dci_type,
                                           dl_msg_alloc&                         msg,
                                           const uci_allocation&                 uci,
                                           unsigned                              pdsch_time_res,
                                           sch_mcs_index                         mcs_idx,
                                           const crb_interval&                   ue_grant_crbs,
                                           const pdsch_config_params&            pdsch_params,
                                           unsigned                              tbs_bytes,
                                           bool                                  is_retx);

  void fill_ul_srb_grant(ue&                                   u,
                         slot_point                            pdcch_slot,
                         std::optional<ul_harq_process_handle> h_ul,
                         pdcch_ul_information&                 pdcch,
                         ul_sched_info&                        msg,
                         unsigned                              pusch_time_res,
                         unsigned                              k2_offset,
                         sch_mcs_index                         mcs_idx,
                         const crb_interval&                   ue_grant_crbs,
                         const pusch_config_params&            pusch_params,
                         unsigned                              tbs_bytes,
                         bool                                  is_retx);

  /// Returns the PDSCH time domain resource allocation for a given PDSCH time resource index.
  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  // Returns the PDSCH time resource index that is suitable for a given PDSCH configuration.
  std::optional<unsigned> get_pdsch_time_res_idx(const pdsch_config_common&                   pdsch_cfg,
                                                 slot_point                                   sl_tx,
                                                 const std::optional<dl_harq_process_handle>& h_dl_retx) const;

  /// Class that keeps track of the UEs' that are waiting for the SRB HARQ processes to be ACKed or retransmitted.
  class ack_and_retx_tracker
  {
  public:
    explicit ack_and_retx_tracker(du_ue_index_t ue_idx, const dl_harq_process_handle& h_dl_) :
      ue_index(ue_idx), h_dl(h_dl_)
    {
    }
    explicit ack_and_retx_tracker(const ack_and_retx_tracker& other) = default;

    bool match_ue_harq(du_ue_index_t ue_idx_, const dl_harq_process_handle& h_dl_) const
    {
      return ue_index == ue_idx_ and h_dl == h_dl_;
    }

    du_ue_index_t ue_index;
    // This field is empty if HARQ is used to schedule ConRes CE only.
    dl_harq_process_handle h_dl;
  };

  void store_harq_tx(du_ue_index_t ue_index, const dl_harq_process_handle& h_dl);

  void rem_fallback_ue(du_ue_index_t ue_index);

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
  uci_allocator&            uci_alloc;
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

  // Bitmap of CRBs that might be used for PUCCH transmissions, to avoid scheduling PUSCH over them.
  crb_bitmap pucch_crbs;

  std::vector<uint8_t> dci_1_0_k1_values;

  srslog::basic_logger& logger;

  /// List of UE's DU Indexes for which SRB0 and SRB1 messages needs to be scheduled.
  std::vector<fallback_ue> pending_dl_ues_new_tx;

  /// List of UE's DU Indexes that are pending for new TX or RE-TX.
  std::vector<du_ue_index_t> pending_ul_ues;
};

} // namespace srsran

/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  void
  handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0, slot_point sl, unsigned srb_buffer_bytes);

  /// Handles UL BSR indication reported by UE.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  void handle_ul_bsr_indication(du_ue_index_t ue_index, unsigned srb_buffer_bytes);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
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

  enum class sched_outcome { success, next_ue, exit_scheduler };

  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  sched_outcome schedule_srb(cell_resource_allocator&       res_alloc,
                             ue&                            u,
                             bool                           is_srb0,
                             dl_harq_process*               h_dl_retx,
                             optional<most_recent_tx_slots> most_recent_tx_ack_slots);

  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_ul_ue(cell_resource_allocator& res_alloc, ue& u, ul_harq_process* h_ul_retx);

  struct sched_srb_results {
    dl_harq_process* h_dl = nullptr;
    // This is only meaningful for SRB1, and represents the number of LCID-1 bytes (excluding any overhead) that have
    // been scheduled for transmission.
    unsigned nof_srb1_scheduled_bytes = 0;
  };

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  sched_srb_results schedule_dl_srb0(ue&                      u,
                                     cell_resource_allocator& res_alloc,
                                     unsigned                 pdsch_time_res,
                                     unsigned                 slot_offset,
                                     slot_point               most_recent_ack_slot,
                                     dl_harq_process*         h_dl_retx);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  sched_srb_results schedule_dl_srb1(ue&                      u,
                                     slot_point               sched_ref_slot,
                                     cell_resource_allocator& res_alloc,
                                     unsigned                 pdsch_time_res,
                                     unsigned                 slot_offset,
                                     slot_point               most_recent_ack_slot,
                                     dl_harq_process*         h_dl_retx = nullptr);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  bool schedule_ul_srb(ue&                                          u,
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
                             bool                       is_srb0);

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
                         unsigned                   tbs_bytes);

  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  optional<unsigned> get_pdsch_time_res_idx(const pdsch_config_common& pdsch_cfg,
                                            slot_point                 sl_tx,
                                            const dl_harq_process*     h_dl_retx) const;

  struct srb_ue {
    du_ue_index_t ue_index;
    bool          is_srb0;
    // Represents the number of LCID-1 bytes (excluding any overhead) that are pending for this UE.
    // This is only meaningful for SRB1 and gets updated every time an RLC buffer state update is received or when we
    // schedule a new PDSCH TX for this UE.
    unsigned pending_srb1_buffer_bytes = 0;
  };

  /// List of UE's DU Indexes for which SRB0 and SRB1 messages needs to be scheduled.
  std::vector<srb_ue> pending_dl_ues_new_tx;

  /// List of UE's DU Indexes for which SRB0 and SRB1 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ul_ues;

  /// Class that keeps track of the UEs' that are waiting for the SRB HARQ processes to be ACKed or retransmitted.
  class ack_and_retx_tracker
  {
  public:
    explicit ack_and_retx_tracker(du_ue_index_t    ue_idx,
                                  dl_harq_process* h_dl_,
                                  bool             is_srb0_,
                                  ue_repository&   ues_,
                                  unsigned         srb_payload_bytes_) :
      ue_index(ue_idx), is_srb0(is_srb0_), h_dl(h_dl_), srb1_payload_bytes(srb_payload_bytes_)
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
    // Represents the number of LCID-1 bytes (excluding any overhead) that have been allocated for this TX.
    // This is only meaningful for SRB1,
    unsigned srb1_payload_bytes = 0;
  };

  void store_harq_tx(du_ue_index_t ue_index, dl_harq_process* h_dl, bool is_srb0, unsigned srb_payload_bytes);

  // If there are any pending SRB0 or SRB1 transmissions for the UE, the function returns the most recent slot with
  // PDSCH for SRB0/SRB1 and the most recent slot with the corresponding PUCCH.
  optional<most_recent_tx_slots> get_most_recent_slot_tx(du_ue_index_t ue_idx) const;

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
  // \param[in] buffer_status_report Number of pending LCID 1 bytes reported by the RLC buffer state update (only LCID 1
  // bytes, without any overhead).
  void update_srb1_buffer_after_rlc_bsu(du_ue_index_t ue_idx, slot_point sl, unsigned buffer_status_report);

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

  // Ring buffer of booleans that indicate whether a slot should be skipped due to lack of PDCCH/PDSCH resources.
  std::array<bool, FALLBACK_SCHED_RING_BUFFER_SIZE> slots_with_no_pdxch_space;
  // Keeps track of the last slot_point used by \ref slots_with_no_pdxch_space.
  slot_point last_sl_ind;

  std::vector<uint8_t> dci_1_0_k1_values;

  srslog::basic_logger& logger;
};

} // namespace srsran

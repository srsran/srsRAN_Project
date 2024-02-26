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
  void handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// Erase the UEs' HARQ processes that have been acked from the SRB scheduler cache.
  void slot_indication();

  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_srb(cell_resource_allocator& res_alloc, ue& u, bool is_srb0, dl_harq_process* h_dl_retx = nullptr);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  dl_harq_process* schedule_srb0(ue&                      u,
                                 cell_resource_allocator& res_alloc,
                                 unsigned                 pdsch_time_res,
                                 unsigned                 slot_offset,
                                 dl_harq_process*         h_dl_retx = nullptr);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  dl_harq_process* schedule_srb1(ue&                      u,
                                 cell_resource_allocator& res_alloc,
                                 unsigned                 pdsch_time_res,
                                 unsigned                 slot_offset,
                                 dl_harq_process*         h_dl_retx = nullptr);

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
                       unsigned                   tbs_bytes,
                       bool                       is_retx);

  void fill_srb1_grant(ue&                        u,
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
                       unsigned                   tbs_bytes,
                       bool                       is_retx);

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
      ue_index(ue_idx), is_srb0(is_srb0_), h_dl(h_dl_), ues(ues_)
    {
      srsran_assert(ues.contains(ue_index), "UE not found in the UE repository");
      srsran_assert(h_dl_ == &(ues[ue_idx].get_pcell().harqs.dl_harq(h_dl_->id)),
                    "HARQ process not found among the UE HARQs");
    }

    bool match_ue_harq(du_ue_index_t ue_idx_, dl_harq_process* h_dl_) const
    {
      return ue_index == ue_idx_ and h_dl == h_dl_;
    }

    // We make the access to the HARQ process is subject to the UE existence in the UE repository.
    dl_harq_process* get_harq_process() const { return ues.contains(ue_index) ? h_dl : nullptr; }

    du_ue_index_t ue_index;
    bool          is_srb0;

  private:
    dl_harq_process* h_dl;
    ue_repository&   ues;
  };

  void cache_ue_srb_tx(du_ue_index_t ue_index, dl_harq_process* h_dl, bool is_srb0);

  const scheduler_ue_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  const unsigned                    max_dl_slots_ahead_sched = 5U;
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

  /// Cache the UEs that are waiting for SRB HARQ processes to be ACKed or retransmitted.
  std::list<ack_and_retx_tracker> ongoing_ues_ack_retx;

  srslog::basic_logger& logger;
};

} // namespace srsran

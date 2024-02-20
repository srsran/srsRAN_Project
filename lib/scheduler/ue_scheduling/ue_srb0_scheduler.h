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

  /// Erase the UEs' HARQ processes that have been acked from the SRB scheduler cache.
  void slot_indication();

  /// Handles DL buffer state reported by upper layers.
  /// \param[in] ue_index UE's DU Index for which SRB0 message needs to be scheduled.
  void handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0);

  /// Schedule UE's SRB0 DL grants for a given slot and one or more cells.
  /// \param[in] res_alloc Resource Grid of the cell where the DL grant is going to be allocated.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_srb0(cell_resource_allocator& res_alloc, ue& u, dl_harq_process* h_dl_retx = nullptr);

  /// \brief Tries to schedule SRB0 message for a UE. Returns true if successful, false otherwise.
  bool schedule_srb1(cell_resource_allocator& res_alloc, ue& u);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space.
  dl_harq_process*
  schedule_srb0(ue& u, cell_resource_allocator& res_alloc, unsigned pdsch_time_res, unsigned slot_offset);

  /// \brief Tries to schedule SRB0 message for a UE and a specific PDSCH TimeDomain Resource and Search Space for a .
  bool schedule_srb0_retx(ue&                      u,
                          cell_resource_allocator& res_alloc,
                          unsigned                 pdsch_time_res,
                          unsigned                 slot_offset,
                          dl_harq_process&         h_dl_retx);

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
                       dl_harq_process*           h_dl_retx = nullptr);

  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const;

  /// Class that keeps track of the UEs' that are waiting for the SRB HARQ processes to be ACKed or retransmitted.
  class ue_srb_tx_tracker
  {
  public:
    explicit ue_srb_tx_tracker(du_ue_index_t ue_index_) : ue_index(ue_index_) {}

    struct srb_dl_harq {
      dl_harq_process* h_dl;
      bool             is_srb0;
    };

    void add_harq_id(dl_harq_process* h_dl, bool is_srb0_)
    {
      if (h_dl == nullptr) {
        return;
      }
      ongoing_h_procs.push_back(srb_dl_harq{h_dl, is_srb0_});
    }

    void erase_acked_processes()
    {
      auto it_h_dl = ongoing_h_procs.begin();

      // Iterate over the HARQ processes, and check if there are any empty HARQ processes.
      while (it_h_dl != ongoing_h_procs.end()) {
        if ((*it_h_dl).h_dl->empty()) {
          it_h_dl = ongoing_h_procs.erase(it_h_dl);
          continue;
        }
        ++it_h_dl;
      }
    }

    bool has_harq_id(harq_id_t h_id) const
    {
      return std::find_if(ongoing_h_procs.begin(), ongoing_h_procs.end(), [h_id](const srb_dl_harq& srb_h_proc) {
               return srb_h_proc.h_dl->id == h_id;
             }) != ongoing_h_procs.end();
    }

    du_ue_index_t            ue_index;
    std::vector<srb_dl_harq> ongoing_h_procs;
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

  /// List of UE's DU Indexes for which SRB0 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ues_srb0;

  /// List of UE's DU Indexes for which SRB1 messages needs to be scheduled.
  std::vector<du_ue_index_t> pending_ues_srb1;

  /// Cache the UEs that are waiting for SRB HARQ processes to be ACKed or retransmitted.
  std::vector<ue_srb_tx_tracker> ongoing_ues;

  srslog::basic_logger& logger;
};

} // namespace srsran

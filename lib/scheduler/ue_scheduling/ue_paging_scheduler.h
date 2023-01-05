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

#include "../cell/cell_configuration.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "ue.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

namespace srsgnb {

/// Defines Paging scheduler that is used to allocate resources to send paging information to UE in a given slot.
class ue_paging_scheduler
{
public:
  explicit ue_paging_scheduler(const scheduler_expert_config&                  expert_cfg_,
                               const cell_configuration&                       cell_cfg_,
                               pdcch_resource_allocator&                       pdcch_sch_,
                               const sched_cell_configuration_request_message& msg);

  /// \brief Performs paging (if any) scheduling for the current slot.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sl_point Slot for which the paging scheduler is called.
  void schedule_paging(cell_slot_resource_allocator& res_grid, slot_point sl_point);

  /// Handles Paging indication message reported by upper layers.
  /// \param[in] paging_indication_message Per UE paging indication message to be scheduled.
  void handle_paging_indication_message(paging_indication_message paging_indication);

private:
  /// \brief Schedules paging for a UE in SearchSpace 0 when pagingSearchSpace is 0 in its active BWP configuration.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sl_point Slot for which the paging scheduler is called.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ue_id UE_ID used in Paging Frame and Paging Occasion formula.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \param[in] paging_msg_size Paging message size.
  bool schedule_paging_in_search_space0(cell_slot_resource_allocator& res_grid,
                                        slot_point                    sl_point,
                                        unsigned                      pdsch_time_res,
                                        unsigned                      ue_id,
                                        unsigned                      i_s,
                                        unsigned                      paging_msg_size);

  /// \brief Searches in PDSCH and PDCCH for space to allocate Paging and Paging's DCI, respectively in SearchSpace 0.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ue_id UE_ID used in Paging Frame and Paging Occasion formula.
  /// \param[in] beam_idx SSB or beam index which the Paging corresponds to.
  /// \param[in] paging_msg_size Paging message size.
  bool allocate_paging_in_search_space0(cell_slot_resource_allocator& res_grid,
                                        unsigned                      pdsch_time_res,
                                        unsigned                      ue_id,
                                        unsigned                      beam_idx,
                                        unsigned                      paging_msg_size);

  /// \brief Fills the Paging n0 slots.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] crbs_grant Paging grant in CRBs.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ue_id UE_ID used in Paging Frame and Paging Occasion formula.
  /// \param[in] dmrs_info DMRS information related to the scheduled grant.
  /// \param[in] tbs TBS information of the Paging grant.
  void fill_paging_grant(cell_slot_resource_allocator& res_grid,
                         crb_interval                  crbs_grant,
                         unsigned                      time_resource,
                         unsigned                      ue_id,
                         const dmrs_information&       dmrs_info,
                         unsigned                      tbs);

  // Args.
  const scheduler_expert_config& expert_cfg;
  const cell_configuration&      cell_cfg;
  pdcch_resource_allocator&      pdcch_sch;

  // Derived args.
  /// Array of Type0-PDCCH CSS monitoring slot n0 (1 per beam) that will be used for Paging scheduling if
  /// pagingSearchSpace is 0 [TS 38.213, Section 13].
  std::array<slot_point, MAX_NUM_BEAMS> type0_pdcch_css_n0_slots;
  /// This is a dummy BWP configuration dimensioned based on CORESET#0 RB limits. It's used for CRB-to-PRB conversion.
  bwp_configuration coreset0_bwp_cfg;
  /// This is a derived parameters, that depends on the SSB periodicity, SIB1 periodicity and SIB1 re-tx periodicity.
  unsigned paging_period;

  /// Default Paging cycle (DRX cycle) configured in SIB1, expressed in nof. radio frames. This value is used if F1AP
  /// does not provide Paging DRX IE in Paging message.
  unsigned default_paging_cycle;
  /// Number of paging frames per DRX cycle. Value of N in the equation in clause 7.1 of TS 38.304.
  unsigned nof_pf_per_drx_cycle;
  /// Paging frame offset. Value of PF_offset in the equation in clause 7.1 of TS 38.304.
  unsigned paging_frame_offset{};
  /// Number of paging occasions per paging frame. Value of Ns in the equation in clause 7.1 of TS 38.304.
  uint8_t nof_po_per_pf;

  /// List of per UE paging indication message yet to be scheduled.
  std::vector<paging_indication_message> paging_pending_ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb

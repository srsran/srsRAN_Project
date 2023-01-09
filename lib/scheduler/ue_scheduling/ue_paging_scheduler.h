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
  /// \brief Schedules paging for a UE in SearchSpace > 0 i.e pagingSearchSpace > 0 in its active BWP configuration.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sl_point Slot for which the paging scheduler is called.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ue_id UE_ID used in Paging Frame and Paging Occasion formula.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \param[in] paging_msg_size Paging message size.
  bool schedule_paging_in_search_space_id_gt_0(cell_slot_resource_allocator& res_grid,
                                               slot_point                    sl_point,
                                               unsigned                      pdsch_time_res,
                                               unsigned                      ue_id,
                                               unsigned                      i_s,
                                               unsigned                      paging_msg_size);

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

  /// \brief Searches in PDSCH and PDCCH for space to allocate Paging and Paging's DCI.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ue_id UE_ID used in Paging Frame and Paging Occasion formula.
  /// \param[in] beam_idx SSB or beam index which the Paging corresponds to.
  /// \param[in] ss_id Search Space Id used in scheduling paging message.
  /// \param[in] paging_msg_size Paging message size.
  bool allocate_paging(cell_slot_resource_allocator& res_grid,
                       unsigned                      pdsch_time_res,
                       unsigned                      ue_id,
                       unsigned                      beam_idx,
                       search_space_id               ss_id,
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

  /// \brief Helper function to precompute Type2-PDCCH Monitoring slots when pagingSearchSpace > 0.
  ///
  /// \param[in] scs_common SCS of PDCCH.
  void precompute_type2_pdcch_slots(subcarrier_spacing scs_common);

  // Args.
  const scheduler_expert_config& expert_cfg;
  const cell_configuration&      cell_cfg;
  pdcch_resource_allocator&      pdcch_sch;

  // Derived args.

  /// Default Paging cycle (DRX cycle) configured in SIB1, expressed in nof. radio frames. This value is used if F1AP
  /// does not provide Paging DRX IE in Paging message.
  unsigned default_paging_cycle;
  /// Number of paging frames per DRX cycle. Value of N in the equation in clause 7.1 of TS 38.304.
  unsigned nof_pf_per_drx_cycle;
  /// Paging frame offset. Value of PF_offset in the equation in clause 7.1 of TS 38.304.
  unsigned paging_frame_offset{};
  /// Number of paging occasions per paging frame. Value of Ns in the equation in clause 7.1 of TS 38.304.
  uint8_t nof_po_per_pf;

  /// Array of Type0-PDCCH CSS monitoring slot n0 (1 per beam) that will be used for Paging scheduling if
  /// pagingSearchSpace is 0 [TS 38.213, Section 13].
  std::array<slot_point, MAX_NUM_BEAMS> type0_pdcch_css_n0_slots;
  /// This is a derived parameters, that depends on the SSB periodicity, SIB1 periodicity and SIB1 re-tx periodicity.
  unsigned sib1_period;

  /// Array of Type2-PDCCH CSS monitoring slots that will be used for Paging scheduling if pagingSearchSpace > 0.
  /// NOTE1: nrofPDCCH-MonitoringOccasionPerSSB-InPO is always 1. See \c
  /// pcch_config::NR_OF_PDCCH_MONITORING_OCCASION_PER_SSB_IN_PO.
  /// NOTE2: Row number corresponds to SSB beam index and column number corresponds to Paging Occasion index (i_s).
  std::array<static_vector<slot_point, pcch_config::MAX_PO_PER_PF>, MAX_NUM_BEAMS> type2_pdcch_css_slots;
  /// Search Space configuration when pagingSearchSpace > 0, if configured.
  search_space_configuration ss_cfg;

  /// BWP configuration of CORESET used for Paging (applies for both pagingSearchSpace = 0 and pagingSearchSpace > 0).
  /// It's used for CRB-to-PRB conversion.
  bwp_configuration coreset_bwp_cfg;

  /// List of per UE paging indication message yet to be scheduled.
  std::vector<paging_indication_message> paging_pending_ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb

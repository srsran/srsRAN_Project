/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include <unordered_map>

namespace srsran {

/// Defines Paging scheduler that is used to allocate resources to send paging information to UE in a given slot.
class paging_scheduler
{
  using five_g_s_tmsi             = uint64_t;
  using full_i_rnti               = uint64_t;
  using paging_retries_count_type = unsigned;

public:
  explicit paging_scheduler(const scheduler_expert_config&                  expert_cfg_,
                            const cell_configuration&                       cell_cfg_,
                            pdcch_resource_allocator&                       pdcch_sch_,
                            const sched_cell_configuration_request_message& msg);

  /// \brief Performs paging (if any) scheduling for the current slot.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  void schedule_paging(cell_resource_allocator& res_grid);

  /// Handles Paging information reported by upper layers.
  /// \param[in] paging_info Per UE paging information to be scheduled.
  void handle_paging_information(const sched_paging_information& paging_info);

private:
  /// \brief Checks paging conditions for a UE in SearchSpace > 0 i.e pagingSearchSpace > 0 in its active BWP config.
  ///
  /// \param[in] pdcch_slot Slot at which the paging scheduler is called.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \return True if paging conditions are satisfied, false otherwise.
  bool is_paging_slot_in_search_space_id_gt_0(slot_point pdcch_slot, unsigned i_s);

  /// \brief Checks paging conditions for a UE in SearchSpace 0 when pagingSearchSpace is 0 in its active BWP config.
  ///
  /// \param[in] sl_point Slot at which the paging scheduler is called.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \return True if paging conditions are satisfied, false otherwise.
  bool is_paging_slot_in_search_space0(slot_point pdcch_slot, unsigned i_s);

  /// \brief Helper function to get sum of paging payload size of each UE scheduled at a particular slot.
  /// \param[in] ues_paging_info List of UE scheduled at a particular slot for Paging.
  /// \return Sum of payload size.
  static unsigned get_accumulated_paging_msg_size(const span<const sched_paging_information>& ues_paging_info);

  /// \brief Checks whether there is space for PDSCH to allocate Paging grant.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] msg_size Paging message size.
  /// \return True if there is space to allocate Paging grant, false otherwise.
  bool
  is_there_space_available_for_paging(cell_resource_allocator& res_grid, unsigned pdsch_time_res, unsigned msg_size);

  /// \brief Allocates PDSCH and PDCCH for Paging.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] pdsch_time_res Slot at which PDSCH needs to be scheduled.
  /// \param[in] ues_paging_info List of UE scheduled at a particular slot for Paging.
  /// \param[in] ss_id Search Space Id used in scheduling paging message.
  /// \return True if paging allocation is successful, false otherwise.
  bool allocate_paging(cell_resource_allocator&                   res_grid,
                       unsigned                                   pdsch_time_res,
                       const span<const sched_paging_information> ues_paging_info,
                       search_space_id                            ss_id);

  /// \brief Fills the Paging grant.
  ///
  /// \param[out,in] pg_grant Paging grant to be filled.
  /// \param[out,in] pdcch Allocated PDCCH for Paging.
  /// \param[in] crbs_grant Paging grant in CRBs.
  /// \param[in] time_resource Slot at which PDSCH needs to be scheduled.
  /// \param[in] ues_paging_info List of UE scheduled at a particular slot for Paging.
  /// \param[in] dmrs_info DMRS information related to the scheduled grant.
  /// \param[in] tbs TBS information of the Paging grant.
  void fill_paging_grant(dl_paging_allocation&                      pg_grant,
                         pdcch_dl_information&                      pdcch,
                         crb_interval                               crbs_grant,
                         unsigned                                   time_resource,
                         const span<const sched_paging_information> ues_paging_info,
                         const dmrs_information&                    dmrs_info,
                         unsigned                                   tbs);

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

  /// Array of Type0-PDCCH CSS monitoring slots (1 per beam) that will be used for Paging scheduling if
  /// pagingSearchSpace is 0 [TS 38.213, Section 13].
  std::array<slot_point, MAX_NUM_BEAMS> type0_pdcch_css_slots;
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
  bwp_configuration bwp_cfg;
  /// PDSCH time domain resource allocation list.
  span<const pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;

  /// List of paging information of UEs yet to be scheduled.
  std::vector<sched_paging_information> paging_pending_ues;
  /// Mapping between NG-5G-S-TMSI (48 bits) to paging retries count for CN Paging.
  std::unordered_map<five_g_s_tmsi, paging_retries_count_type> cn_paging_retries;
  /// Mapping between I-RNTI-Value (40 bits) to paging retries count for RAN Paging.
  std::unordered_map<full_i_rnti, paging_retries_count_type> ran_paging_retries;

  srslog::basic_logger& logger;
};

} // namespace srsran

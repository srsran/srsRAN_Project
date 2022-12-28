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
  explicit ue_paging_scheduler(const scheduler_ue_expert_config&               expert_cfg_,
                               const cell_configuration&                       cell_cfg_,
                               pdcch_resource_allocator&                       pdcch_sch_,
                               ue_list&                                        ues_,
                               const sched_cell_configuration_request_message& msg);

private:
  // Args.
  const scheduler_ue_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  ue_list&                          ues;

  /// Vector of Type0-PDCCH CSS monitoring slot n0 (1 per beam) that will be used
  /// for Paging scheduling if pagingSearchSpace is 0 [TS 38.213, Section 13].
  static_vector<slot_point, MAX_NUM_BEAMS> type0_pdcch_css_n0_slots;
  /// This is a dummy BWP configuration dimensioned based on CORESET#0 RB limits. It's used for CRB-to-PRB conversion.
  bwp_configuration coreset0_bwp_cfg;

  /// Default Paging cycle configured in SIB1, expressed in nof. radio frames.
  unsigned sib1_default_paging_cycle;

  bwp_configuration initial_active_dl_bwp;

  srslog::basic_logger& logger;
};

} // namespace srsgnb

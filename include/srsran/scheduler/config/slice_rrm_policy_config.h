/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/rrm.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"

namespace srsran {

/// Cell-specific Default RAN slice configuration.
struct slice_rrm_policy_config {
  /// Maximum RAN scheduling policy.
  constexpr static unsigned max_priority = 255;

  /// RRM Policy identifier.
  rrm_policy_member rrc_member;
  /// Sets the minimum number of PRBs to be allocated to this group.
  unsigned min_prb = 0;
  /// Sets the maximum number of PRBs to be allocated to this group.
  unsigned max_prb = MAX_NOF_PRBS;
  /// RAN slice scheduling priority. Values: {0, ..., 255}.
  unsigned priority = 0;
  /// Policy scheduler configuration for the slice.
  policy_scheduler_expert_config policy_sched_cfg = time_rr_scheduler_expert_config{};
};

} // namespace srsran

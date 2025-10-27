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

#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/rrm.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"

namespace srsran {

/// Cell-specific Default RAN slice configuration.
struct slice_rrm_policy_config {
  /// Maximum RAN scheduling policy.
  static constexpr unsigned max_priority = 255;

  /// RRM Policy identifier.
  rrm_policy_member rrc_member;
  /// \brief Radio resources, in RBs, allocated to this group.
  ///
  /// These limits are computed based on the rrmPolicyRatios provided for the RRM policy and based on the number of RBs
  /// of the cell.
  rrm_policy_ratio_rb_limits rbs;
  /// RAN slice scheduling priority. Values: {0, ..., 255}.
  unsigned priority = 0;
  /// Policy scheduler configuration for the slice.
  scheduler_policy_config policy_sched_cfg = time_qos_scheduler_config{};
};

} // namespace srsran

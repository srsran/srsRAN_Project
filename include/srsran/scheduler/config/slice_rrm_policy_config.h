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
  rrm_policy_radio_block_limits rbs;
  /// RAN slice scheduling priority. Values: {0, ..., 255}.
  unsigned priority = 0;
  /// Policy scheduler configuration for the slice.
  policy_scheduler_expert_config policy_sched_cfg = time_qos_scheduler_expert_config{};
};

} // namespace srsran

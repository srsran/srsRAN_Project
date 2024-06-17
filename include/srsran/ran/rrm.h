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

#include "srsran/ran/s_nssai.h"

namespace srsran {

/// Identifier of a RRM Policy Member.
/// \remark See O-RAN.WG3.E2SM-RC-R003-v3.00 Section 8.4.3.6
struct rrm_policy_member {
  std::string plmn_id;
  /// Single Network Slice Selection Assistance Information (S-NSSAI).
  s_nssai_t s_nssai;

  bool operator==(const rrm_policy_member& other) const
  {
    return plmn_id == other.plmn_id and s_nssai == other.s_nssai;
  }
};

struct rrm_policy_ratio_group {
  /// Used to identify the group to which the policy is applied.
  rrm_policy_member pol_member;
  /// Sets the minimum percentage of PRBs to be allocated to this group.
  std::optional<int> min_prb_policy_ratio;
  /// Sets the maximum percentage of PRBs to be allocated to this group.
  std::optional<int> max_prb_policy_ratio;
  /// Sets the percentage of PRBs to be allocated to this group.
  std::optional<int> ded_prb_policy_ratio;
};

} // namespace srsran

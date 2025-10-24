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

#include "srsran/ran/du_types.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/s_nssai.h"

namespace srsran {

/// Identifier of a RRM Policy Member.
struct rrm_policy_member {
  plmn_identity plmn_id = plmn_identity::test_value();
  /// Single Network Slice Selection Assistance Information (S-NSSAI).
  s_nssai_t s_nssai;

  bool operator==(const rrm_policy_member& other) const
  {
    return plmn_id == other.plmn_id and s_nssai == other.s_nssai;
  }
};

/// Resource configuration of a RRM Policy Ratio Group.
struct rrm_policy_ratio_group {
  /// The resource type of interest for an RRM Policy.
  /// \remark See 3GPP TS 28.541, Section 4.4.1 Attribute properties.
  enum class resource_type_t { prb, prb_ul, prb_dl };
  resource_type_t resource_type = resource_type_t::prb;
  /// List of RRM policy members (PLMN + S-NSSAI combinations).
  std::vector<rrm_policy_member> policy_members_list;
  /// The percentage of dedicated PRBs to be allocated to this group.
  std::optional<unsigned> dedicated_ratio;
  /// The percentage of prioritized + dedicated PRBs to be allocated to this group.
  std::optional<unsigned> minimum_ratio;
  /// The percentage of dedicated + prioritized + shared PRBs to be allocated to this group.
  std::optional<unsigned> maximum_ratio;
};

/// Number of dedicated, prioritized and shared PRBs associated with a given RAN slice.
struct rrm_policy_radio_block_limits {
  rrm_policy_radio_block_limits() = default;
  rrm_policy_radio_block_limits(unsigned min_rbs_, unsigned max_rbs_) :
    rrm_policy_radio_block_limits(0, min_rbs_, max_rbs_)
  {
  }
  rrm_policy_radio_block_limits(unsigned ded_rbs_, unsigned min_rbs_, unsigned max_rbs_) :
    ded_rbs(ded_rbs_), min_rbs(min_rbs_), max_rbs(max_rbs_)
  {
    srsran_assert(ded_rbs_ <= min_rbs_, "Invalid RB limits: dedicated {} > minimum {}", ded_rbs_, min_rbs_);
    srsran_assert(min_rbs_ <= max_rbs_, "Invalid RB limits: minimum {} > maximum {}", min_rbs_, max_rbs_);
  }

  /// Sum of dedicated and prioritized RBs.
  unsigned min() const { return min_rbs; }

  /// Sum of dedicated, prioritized and shared RBs.
  unsigned max() const { return max_rbs; }

  /// Dedicated PRBs.
  unsigned dedicated() const { return ded_rbs; }

  /// Prioritized PRBs.
  unsigned prioritized() const { return min_rbs - ded_rbs; }

  /// Shared PRBs.
  unsigned shared() const { return max_rbs - min_rbs; }

private:
  unsigned ded_rbs = 0;
  unsigned min_rbs = 0;
  unsigned max_rbs = MAX_NOF_PRBS;
};

constexpr unsigned MAX_SLICE_RECONF_POLICIES = 16;

/// Request to reconfigure slicing policies for a given DU cell.
struct du_cell_slice_reconfig_request {
  du_cell_index_t cell_index;
  struct rrm_policy_config {
    rrm_policy_member rrc_member;
    /// RB limits for the RRM policy member.
    rrm_policy_radio_block_limits rbs;
  };
  static_vector<rrm_policy_config, MAX_SLICE_RECONF_POLICIES> rrm_policies;
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rrm_policy_member> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rrm_policy_member& member, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "{{PLMN={}, SST={}, SD={}}}",
                     member.plmn_id.to_string(),
                     member.s_nssai.sst.value(),
                     member.s_nssai.sd.value());
  }
};

} // namespace fmt

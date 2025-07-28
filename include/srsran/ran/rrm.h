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

#include "srsran/ran/plmn_identity.h"
#include "srsran/ran/s_nssai.h"

namespace srsran {

/// Identifier of a RRM Policy Member.
/// \remark See O-RAN.WG3.E2SM-RC-R003-v3.00 Section 8.4.3.6
struct rrm_policy_member {
  plmn_identity plmn_id = plmn_identity::test_value();
  /// Single Network Slice Selection Assistance Information (S-NSSAI).
  s_nssai_t s_nssai;

  bool operator==(const rrm_policy_member& other) const
  {
    return plmn_id == other.plmn_id and s_nssai == other.s_nssai;
  }
};

struct rrm_policy_ratio_group {
  /// The resource type of interest for an RRM Policy
  /// \remark See 3GPP TS 28.541, Section 4.4.1 Attribute properties.
  enum class resource_type_t { prb, prb_ul, prb_dl };
  resource_type_t resource_type = resource_type_t::prb;
  /// List of RRM policy members (PLMN + S-NSSAI combinations).
  std::vector<rrm_policy_member> policy_members_list;
  /// Sets the minimum percentage of PRBs to be allocated to this group.
  std::optional<unsigned> min_prb_policy_ratio;
  /// Sets the maximum percentage of PRBs to be allocated to this group.
  std::optional<unsigned> max_prb_policy_ratio;
  /// Sets the percentage of PRBs to be allocated to this group.
  std::optional<unsigned> ded_prb_policy_ratio;
};

} // namespace srsran

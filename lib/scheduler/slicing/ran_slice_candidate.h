/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ran_slice_instance.h"

namespace srsran {
namespace detail {

/// \brief RAN slice that is the next candidate for allocation in a given slot and cell.
template <bool IsDl>
class common_ran_slice_candidate
{
public:
  common_ran_slice_candidate(ran_slice_instance& instance_, unsigned max_rbs_ = 0) :
    inst(&instance_), max_rbs(max_rbs_ == 0 ? inst->cfg.max_prb : max_rbs_)
  {
  }

  ran_slice_id_t                               id() const { return inst->id; }
  [[nodiscard]] const slice_rrm_policy_config& cfg() const { return inst->cfg; }

  bool is_candidate(du_ue_index_t ue_idx) const { return inst->contains(ue_idx); }
  bool is_candidate(du_ue_index_t ue_idx, lcid_t lcid) const { return inst->contains(ue_idx, lcid); }

  /// Get UEs belonging to a slice.
  const slice_ue_repository& get_slice_ues() const { return inst->get_ues(); }

  /// Register that a new grant was allocated for a given UE.
  void store_grant(unsigned nof_rbs)
  {
    if constexpr (IsDl) {
      inst->store_pdsch_grant(nof_rbs);
    } else {
      inst->store_pusch_grant(nof_rbs);
    }
  }

  /// Remaining bytes to allocate for the given slice.
  [[nodiscard]] unsigned remaining_rbs() const
  {
    if constexpr (IsDl) {
      return max_rbs < inst->pdsch_rb_count ? 0 : max_rbs - inst->pdsch_rb_count;
    }
    return max_rbs < inst->pusch_rb_count ? 0 : max_rbs - inst->pusch_rb_count;
  }

protected:
  ran_slice_instance* inst    = nullptr;
  unsigned            max_rbs = 0;
};

} // namespace detail

/// \brief Handle to fetch and update the state of a RAN slice in a given DL slot.
///
/// On destruction, the slice is marked as completed for the current slot and won't be considered as a candidate again.
using dl_ran_slice_candidate = detail::common_ran_slice_candidate<true>;

/// Interface to fetch and update the state of a RAN slice in a given UL slot.
///
/// On destruction, the slice is marked as completed for the current slot and won't be considered as a candidate again.
using ul_ran_slice_candidate = detail::common_ran_slice_candidate<false>;

} // namespace srsran
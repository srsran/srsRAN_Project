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

#include "ran_slice_instance.h"

namespace srsran {
namespace detail {

template <bool IsDl>
class common_ran_slice_candidate
{
  struct candidate_deleter {
    void operator()(ran_slice_instance* p)
    {
      if (p != nullptr) {
        if constexpr (IsDl) {
          p->pdsch_completed();
        } else {
          p->pusch_completed();
        }
      }
    }
  };

public:
  common_ran_slice_candidate(ran_slice_instance* instance_) : inst(instance_, candidate_deleter{}) {}

  ran_slice_id_t                              id() const { return inst->id; }
  [[nodiscard]] const cell_rrm_policy_config& cfg() const { return inst->cfg; }
  scheduler_policy&                           policy() { return *inst->policy; }

  bool is_candidate(du_ue_index_t ue_idx) const { return inst->is_candidate(ue_idx); }
  bool is_candidate(du_ue_index_t ue_idx, lcid_t lcid) const { return inst->is_candidate(ue_idx, lcid); }

  /// Signal that the allocations for this slice are complete.
  void clear() { inst.reset(); }

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
      return inst->cfg.max_prb_ratio < inst->pdsch_rb_count ? 0 : inst->cfg.max_prb_ratio - inst->pdsch_rb_count;
    }
    return inst->cfg.max_prb_ratio < inst->pusch_rb_count ? 0 : inst->cfg.max_prb_ratio - inst->pusch_rb_count;
  }

protected:
  std::unique_ptr<ran_slice_instance, candidate_deleter> inst;
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
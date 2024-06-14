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

#include "../policy/scheduler_policy.h"
#include "ran_slice_candidate.h"
#include "ran_slice_instance.h"

namespace srsran {

/// Inter-slice Scheduler.
class slice_scheduler
{
public:
  slice_scheduler(const cell_configuration& cell_cfg_, const slice_rrm_policy_config& cfg_);

  /// Reset the state of the slices.
  void slot_indication();

  /// Update the state of the slice with the provided UE configs.
  void add_ue(du_ue_index_t ue_idx, const ue_configuration& ue_cfg);
  void reconf_ue(du_ue_index_t ue_idx, const ue_configuration& next_ue_cfg, const ue_configuration& prev_ue_cfg);
  void rem_ue(du_ue_index_t ue_idx);

  /// Get next RAN slice for PDSCH scheduling.
  dl_ran_slice_candidate get_next_dl_candidate();

  /// Get next RAN slice for PUSCH scheduling.
  ul_ran_slice_candidate get_next_ul_candidate();

private:
  struct slice_prio_context {
    ran_slice_id_t id;
    // Cached values.
    int prio = 0;

    slice_prio_context(ran_slice_id_t id_) : id(id_) {}

    /// Compares priorities between two slice contexts.
    bool operator<(const slice_prio_context& rhs) const { return prio < rhs.prio; }
    bool operator>(const slice_prio_context& rhs) const { return prio > rhs.prio; }
  };

  ran_slice_instance& get_slice(const rrm_policy_member& rrm);

  dl_ran_slice_candidate create_dl_candidate();
  ul_ran_slice_candidate create_ul_candidate();

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  std::vector<ran_slice_instance> slices;

  /// List of slice IDs sorted by priority.
  std::vector<slice_prio_context> sorted_dl_prios;
  std::vector<slice_prio_context> sorted_ul_prios;
};

} // namespace srsran
/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/unique_thread.h"

namespace srsran {

/// Types of CPU affinity masks in the gNB.
enum class gnb_sched_affinity_mask_types { l1_dl, l1_ul, l2_cell, ru, low_priority, last };

inline unsigned to_unsigned(gnb_sched_affinity_mask_types value)
{
  return static_cast<unsigned>(value);
}

inline gnb_sched_affinity_mask_types to_affinity_mask_type(unsigned value)
{
  return static_cast<gnb_sched_affinity_mask_types>(value);
}

/// \brief Scheduler affinity mask manager.
///
/// Manages the CPU affinity mask for the gNB application. It calculates and returns a mask using on a round-robin
/// algorithm on a configured mask.
class gnb_os_sched_affinity_manager
{
  /// Helper class that manages the mask and implements the round-robin algorithm.
  class affinity_entry
  {
  public:
    explicit affinity_entry(const os_sched_affinity_bitmask& mask_) : mask(mask_) {}

    /// Calculates and returns a bitmask using a round-robin algorithm over the configured mask. If no CPU is enabled in
    /// the configured mask, it returns the default mask.
    os_sched_affinity_bitmask calculate_mask()
    {
      // No CPU is selected.
      if (mask.none()) {
        return {};
      }

      int pos = -1;

      while (pos < 0) {
        pos = mask.find_lowest(last_pos_used, mask.size());

        last_pos_used = (pos < 0) ? 0 : pos + 1;
      }

      return os_sched_affinity_bitmask(pos);
    }

  private:
    os_sched_affinity_bitmask mask;
    unsigned                  last_pos_used = 0;
  };

public:
  gnb_os_sched_affinity_manager(const std::vector<os_sched_affinity_bitmask>& entries)
  {
    srsran_assert(entries.size() == to_unsigned(gnb_sched_affinity_mask_types::last),
                  "Invalid number of CPU affinity masks received. Got '{}' while expected '{}' ",
                  entries.size(),
                  to_unsigned(gnb_sched_affinity_mask_types::last));

    for (const auto& entry : entries) {
      cpu_masks.emplace_back(entry);
    }
  }

  /// Calculate and returns the affinity mask for the given type.
  os_sched_affinity_bitmask calcute_affinity_mask(gnb_sched_affinity_mask_types type)
  {
    srsran_assert(type != gnb_sched_affinity_mask_types::last, "Invalid type '{}'", to_unsigned(type));

    return cpu_masks[to_unsigned(type)].calculate_mask();
  }

private:
  std::vector<affinity_entry> cpu_masks;
};

} // namespace srsran

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

#include "srsran/support/unique_thread.h"

namespace srsran {

/// Types of CPU affinity masks in the gNB.
enum class gnb_sched_affinity_mask_types { l1_dl, l1_ul, l2_cell, ru, low_priority, last };

/// Thread pinning policy to a CPU affinity mask in the gNB.
enum class gnb_sched_affinity_mask_policy {
  /// Each thread is pinned to a single CPU in a mask in RR fashion.
  round_robin = 0,
  /// All threads are sharing the same affinity mask.
  mask = 1,
  last
};

/// Converts the given sting into an affinity mask policy or returns last if it could not convert it.
inline gnb_sched_affinity_mask_policy to_affinity_mask_policy(const std::string& value)
{
  if (value == "round-robin") {
    return gnb_sched_affinity_mask_policy::round_robin;
  }
  if (value == "mask") {
    return gnb_sched_affinity_mask_policy::mask;
  }

  return gnb_sched_affinity_mask_policy::last;
}

/// Converts and returns the given affinity mask type to an integer.
inline unsigned to_unsigned(gnb_sched_affinity_mask_types value)
{
  return static_cast<unsigned>(value);
}

/// Converts and returns the given value to an affinity mask type.
inline gnb_sched_affinity_mask_types to_affinity_mask_type(unsigned value)
{
  return static_cast<gnb_sched_affinity_mask_types>(value);
}

/// CPU affinity configuration in the gNB app.
struct gnb_os_sched_affinity_config {
  /// Affinity mask.
  os_sched_affinity_bitmask mask;
  /// Thread pinning policy.
  gnb_sched_affinity_mask_policy pinning_policy = gnb_sched_affinity_mask_policy::mask;
};

/// \brief Scheduler affinity mask manager.
///
/// Manages the CPU affinity mask for the gNB application. It calculates and returns a mask using on a round-robin
/// algorithm on a configured mask.
class gnb_os_sched_affinity_manager
{
  /// Interface of the class managing affinity mask assignments depending on the policy.
  class affinity_entry
  {
  public:
    explicit affinity_entry(const os_sched_affinity_bitmask& mask_) : mask(mask_) {}
    virtual ~affinity_entry() = default;

    virtual os_sched_affinity_bitmask calculate_mask() = 0;

  protected:
    os_sched_affinity_bitmask mask;
  };

  /// Helper class that manages the mask and implements the round-robin algorithm.
  class affinity_entry_rr : public affinity_entry
  {
  public:
    explicit affinity_entry_rr(const os_sched_affinity_bitmask& mask_) : affinity_entry(mask_) {}

    /// Calculates and returns a bitmask using a round-robin algorithm over the configured mask. If no CPU is enabled in
    /// the configured mask, it returns the default mask.
    os_sched_affinity_bitmask calculate_mask() override
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

      return os_sched_affinity_bitmask(mask.size(), pos);
    }

  private:
    unsigned last_pos_used = 0;
  };

  /// Helper class that manages the mask and return user-defined mask.
  class affinity_entry_mask : public affinity_entry
  {
  public:
    explicit affinity_entry_mask(const os_sched_affinity_bitmask& mask_) : affinity_entry(mask_) {}

    os_sched_affinity_bitmask calculate_mask() override { return mask; }
  };

public:
  gnb_os_sched_affinity_manager(const std::vector<gnb_os_sched_affinity_config>& entries)
  {
    srsran_assert(entries.size() == to_unsigned(gnb_sched_affinity_mask_types::last),
                  "Invalid number of CPU affinity masks received. Got '{}' while expected '{}' ",
                  entries.size(),
                  to_unsigned(gnb_sched_affinity_mask_types::last));

    for (const auto& entry : entries) {
      switch (entry.pinning_policy) {
        case gnb_sched_affinity_mask_policy::round_robin:
          cpu_masks.push_back(std::make_unique<affinity_entry_rr>(entry.mask));
          break;
        case gnb_sched_affinity_mask_policy::mask:
          cpu_masks.push_back(std::make_unique<affinity_entry_mask>(entry.mask));
          break;
        default:
          srsran_assert(0, "Invalid affinity mask policy={}", entry.pinning_policy);
      }
    }
  }

  /// Calculate and returns the affinity mask for the given type.
  os_sched_affinity_bitmask calcute_affinity_mask(gnb_sched_affinity_mask_types type)
  {
    srsran_assert(type != gnb_sched_affinity_mask_types::last, "Invalid type '{}'", to_unsigned(type));

    return cpu_masks[to_unsigned(type)]->calculate_mask();
  }

private:
  std::vector<std::unique_ptr<affinity_entry>> cpu_masks;
};

} // namespace srsran

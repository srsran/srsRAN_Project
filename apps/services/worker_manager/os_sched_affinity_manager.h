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

#include "srsran/support/executors/unique_thread.h"

namespace srsran {

/// Types of CPU affinity masks.
enum class sched_affinity_mask_types { l1_dl, ru, main, last };

/// Thread pinning policy to a CPU affinity mask.
enum class sched_affinity_mask_policy {
  /// Each thread is pinned to a single CPU in a mask in RR fashion.
  round_robin = 0,
  /// All threads are sharing the same affinity mask.
  mask = 1,
  last
};

/// Converts the given affinity mask policy into a string.
inline std::string to_string(sched_affinity_mask_policy policy)
{
  switch (policy) {
    case sched_affinity_mask_policy::mask:
      return "mask";
    case sched_affinity_mask_policy::round_robin:
      return "round-robin";
    default:
      srsran_assert(0, "Invalid affinity mask policy");
      break;
  }
  return {};
}

/// Converts the given sting into an affinity mask policy or returns last if it could not convert it.
inline sched_affinity_mask_policy to_affinity_mask_policy(const std::string& value)
{
  if (value == "round-robin") {
    return sched_affinity_mask_policy::round_robin;
  }
  if (value == "mask") {
    return sched_affinity_mask_policy::mask;
  }

  return sched_affinity_mask_policy::last;
}

/// Converts and returns the given affinity mask type to an integer.
inline unsigned to_unsigned(sched_affinity_mask_types value)
{
  return static_cast<unsigned>(value);
}

/// Converts and returns the given value to an affinity mask type.
inline sched_affinity_mask_types to_affinity_mask_type(unsigned value)
{
  return static_cast<sched_affinity_mask_types>(value);
}

/// CPU affinity configuration.
struct os_sched_affinity_config {
  /// Affinity type.
  sched_affinity_mask_types type = sched_affinity_mask_types::last;
  /// Affinity mask.
  os_sched_affinity_bitmask mask;
  /// Thread pinning policy.
  sched_affinity_mask_policy pinning_policy = sched_affinity_mask_policy::last;
};

/// \brief Scheduler affinity mask manager.
///
/// Manages the CPU affinity mask for the application. It calculates and returns a mask using on a round-robin algorithm
/// on a configured mask.
class os_sched_affinity_manager
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
  os_sched_affinity_manager(const std::vector<os_sched_affinity_config>& entries) :
    cpu_masks(to_unsigned(sched_affinity_mask_types::last))
  {
    for (const auto& entry : entries) {
      srsran_assert(entry.type != sched_affinity_mask_types::last, "Invalid type '{}'", to_unsigned(entry.type));
      switch (entry.pinning_policy) {
        case sched_affinity_mask_policy::round_robin:
          cpu_masks[to_unsigned(entry.type)] = std::make_unique<affinity_entry_rr>(entry.mask);
          break;
        case sched_affinity_mask_policy::mask:
          cpu_masks[to_unsigned(entry.type)] = std::make_unique<affinity_entry_mask>(entry.mask);
          break;
        default:
          cpu_masks[to_unsigned(entry.type)] = nullptr;
      }
    }
  }

  /// Calculate and returns the affinity mask for the given type.
  os_sched_affinity_bitmask calcute_affinity_mask(sched_affinity_mask_types type) const
  {
    srsran_assert(type != sched_affinity_mask_types::last, "Invalid type '{}'", to_unsigned(type));
    srsran_assert(cpu_masks[to_unsigned(type)], "Invalid manager for type '{}'", to_unsigned(type));

    return cpu_masks[to_unsigned(type)]->calculate_mask();
  }

private:
  std::vector<std::unique_ptr<affinity_entry>> cpu_masks;
};

} // namespace srsran

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

#include "srsran/ran/slot_point.h"
#include <atomic>

namespace srsran {

/// Atomically compares and updates the values of the passed atomics if the respective condition is true.
inline void update_minmax(uint64_t new_value, std::atomic<uint64_t>& max, std::atomic<uint64_t>& min)
{
  auto current_min = min.load(std::memory_order_relaxed);
  auto current_max = max.load(std::memory_order_relaxed);
  while (new_value < current_min && !min.compare_exchange_weak(current_min, new_value)) {
  }
  while (new_value > current_max && !max.compare_exchange_weak(current_max, new_value)) {
  }
}

constexpr uint64_t pack_slot_and_duration(slot_point slot, uint64_t duration_ns)
{
  // Pack duration as 32 bits, maximum of approximately 4.29 seconds.
  uint64_t packed = std::min(duration_ns, 0xffffffffUL);

  // Pack slot index which takes no more than 20 bits.
  packed |= (slot.to_uint() & 0xfffffUL) << 32UL;

  // Pack numerology as 4 bits.
  packed |= (slot.numerology() & 0xfUL) << 60UL;

  return packed;
}

constexpr uint64_t unpack_duration(uint64_t packed)
{
  return packed & 0xffffffffUL;
}

constexpr slot_point unpack_slot(uint64_t packed)
{
  uint64_t numerology = packed >> 60UL;
  uint64_t slot_index = (packed >> 32UL) & 0xfffffUL;

  // Detect invalid numerology.
  if (numerology >= to_numerology_value(subcarrier_spacing::invalid)) {
    return {};
  }

  return slot_point(numerology, slot_index);
}

/// Atomically compares and updates the values of the passed atomics if the respective condition is true.
inline void update_slotmax(slot_point slot, uint64_t duration_ns, std::atomic<uint64_t>& max_packed)
{
  uint64_t new_packed         = pack_slot_and_duration(slot, duration_ns);
  auto     current_max_packed = max_packed.load(std::memory_order_relaxed);
  while (duration_ns > unpack_duration(current_max_packed) &&
         !max_packed.compare_exchange_weak(current_max_packed, new_packed)) {
  }
}

/// Atomically compares and updates the values of the passed atomics if the respective condition is true.
inline void update_slotmin(slot_point slot, uint64_t duration_ns, std::atomic<uint64_t>& min_packed)
{
  uint64_t new_packed         = pack_slot_and_duration(slot, duration_ns);
  auto     current_max_packed = min_packed.load(std::memory_order_relaxed);
  while (duration_ns < unpack_duration(current_max_packed) &&
         !min_packed.compare_exchange_weak(current_max_packed, new_packed)) {
  }
}

} // namespace srsran

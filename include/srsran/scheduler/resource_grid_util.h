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

#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/math/math_utils.h"

namespace srsran {

/// \brief Derives a ring size for the resource grid allocator that is equal or larger than the given minimum value.
/// \remark 1. The ring size must satisfy the condition NOF_SLOTS_PER_HYPER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0, for
/// the used numerology. Otherwise, misalignments may occur close to the slot point wrap around.
/// Misalignment example: Assume NOF_SLOTS_PER_HYPER_SYSTEM_FRAME = 10240 and RING_ALLOCATOR_SIZE = 37
/// At the slot 1023.9, the ring index 10239 % 37 = 26 is accessed. At slot point 0.0 (once slot point wraps around),
/// the ring index 0 % 37 = 0 would be accessed.
/// \remark 2. If the condition NOF_SLOTS_PER_HYPER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0 is satisfied for
/// the numerology mu=0 (SCS=15kHz), it will be also satisfied for the same RING_ALLOCATOR_SIZE and larger numerologies.
/// This means that in contexts where mu is not known (e.g. compile time), mu=0 can be used for generality sake,
/// at the expense of more memory overhead.
constexpr unsigned get_allocator_ring_size_gt_min(unsigned           minimum_value,
                                                  subcarrier_spacing scs = subcarrier_spacing::kHz15)
{
  auto power2_ceil = [](unsigned x) {
    if (x <= 1)
      return 1U;
    unsigned power = 2;
    x--;
    while (x >>= 1)
      power <<= 1;
    return power;
  };

  unsigned slots_per_frame = 10U * get_nof_slots_per_subframe(scs);
  unsigned frames_ceil     = divide_ceil(minimum_value, slots_per_frame);
  return power2_ceil(frames_ceil) * slots_per_frame;
}

/// \brief Retrieves how far in advance the scheduler can allocate resources in the UL resource grid.
constexpr unsigned get_max_slot_ul_alloc_delay(unsigned ntn_cs_koffset)
{
  return SCHEDULER_MAX_K0 + std::max(SCHEDULER_MAX_K1, SCHEDULER_MAX_K2 + MAX_MSG3_DELTA) + ntn_cs_koffset;
}

} // namespace srsran

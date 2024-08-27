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

#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// \brief Determines a the resource grid allocator ring size that is greater than the given minimum value.
/// \remark 1. The ring size must satisfy the condition NOF_SLOTS_PER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0, for
/// the used numerology. Otherwise, misalignments may occur close to the slot point wrap around.
/// Misalignment example: Assume NOF_SLOTS_PER_SYSTEM_FRAME = 10240 and RING_ALLOCATOR_SIZE = 37
/// At the slot 1023.9, the ring index 10239 % 37 = 26 is accessed. At slot point 0.0 (once slot point wraps around),
/// the ring index 0 % 37 = 0 would be accessed.
constexpr inline unsigned get_allocator_ring_size_gt_min(unsigned minimum_value)
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

  // Note: we compute ring size assuming numerology 0. The reason being that if the condition
  // NOF_SLOTS_PER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0 is true for numerology 0, it will be as well for other
  // numerologies.
  unsigned div10 = divide_ceil(minimum_value, 10U);
  return power2_ceil(div10) * 10;
}

/// \brief Retrieves how far in advance the scheduler can allocate resources in the UL resource grid.
constexpr inline unsigned get_max_slot_ul_alloc_delay(unsigned ntn_cs_koffset)
{
  return SCHEDULER_MAX_K0 + std::max(SCHEDULER_MAX_K1, SCHEDULER_MAX_K2 + MAX_MSG3_DELTA) + ntn_cs_koffset;
}

} // namespace srsran

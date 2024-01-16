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

namespace srsran {

/// \brief Retrieves the resource grid allocator ring size greater than given minimum value.
/// \remark 1. The implementation of circular ring based resource allocator only works correctly if we set a
/// ring size which satisfies the condition NOF_SLOTS_PER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0.
/// The condition is placed to avoid misalignment between (last_slot_ind + slot_delay) slot point and
/// slot point contained in ((last_slot_ind + slot_delay) % RING_ALLOCATOR_SIZE) slot, which occurs when slot point is
/// close to NOF_SLOTS_PER_SYSTEM_FRAME value.
/// Misalignment example: Assume NOF_SLOTS_PER_SYSTEM_FRAME = 10240 and RING_ALLOCATOR_SIZE = 37
/// At Slot point (10238) % 37 = Element at index 26 of slots array/vector is accessed, similarly
/// Slot point (10239) % 37 = 27
/// Now, Slot point wraps around NOF_SLOTS_PER_SYSTEM_FRAME and is set to 0, this causes Slot point (0) % 37 = 0.
/// Resulting in element at index 0 of slots array/vector being accessed rather than index 28.
/// \remark 2. The reason for choosing values 20, 40 and 80 for RING_ALLOCATOR_SIZE is because it holds the condition
/// NOF_SLOTS_PER_SYSTEM_FRAME % RING_ALLOCATOR_SIZE = 0. for all numerologies.
constexpr inline unsigned get_allocator_ring_size_gt_min(unsigned minimum_value)
{
  if (minimum_value < 20) {
    return 20;
  }
  if (minimum_value < 40) {
    return 40;
  }
  return 640;
}

} // namespace srsran

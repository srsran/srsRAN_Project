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

namespace srsran {
namespace ofh {

/// System frame number maximum value in the Open Fronthaul repositories.
constexpr unsigned SFN_MAX_VALUE = 1U << 8;

/// \brief Calculates the size of the repository using the given parameters.
///
/// This function will find the minimum divisible value of the maximum number of slots that Open Fronthaul supports.
constexpr unsigned calculate_repository_size(subcarrier_spacing scs, unsigned minimum_repo_size)
{
  for (unsigned nof_slots = SFN_MAX_VALUE * get_nof_slots_per_subframe(scs) * NOF_SUBFRAMES_PER_FRAME;
       nof_slots % minimum_repo_size != 0;
       ++minimum_repo_size) {
  }

  return minimum_repo_size;
}

/// Calculates the repository local index for the given slot and repository size.
constexpr unsigned calculate_repository_index(slot_point slot, unsigned repo_size)
{
  slot_point entry_slot(slot.numerology(), slot.sfn() % SFN_MAX_VALUE, slot.slot_index());
  return entry_slot.system_slot() % repo_size;
}

} // namespace ofh
} // namespace srsran

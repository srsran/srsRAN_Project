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
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/test_utils.h"

namespace srsran {
namespace test_helper {

inline slot_point generate_random_slot_point(subcarrier_spacing scs)
{
  static std::array<std::uniform_int_distribution<uint32_t>, NOF_NUMEROLOGIES> scs_dists = {
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz15)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz30)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz60)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz120)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz240)) - 1}};

  uint32_t count = scs_dists[to_numerology_value(scs)](test_rgen::get());
  return slot_point{scs, count};
}

} // namespace test_helper
} // namespace srsran

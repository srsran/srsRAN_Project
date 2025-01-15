/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

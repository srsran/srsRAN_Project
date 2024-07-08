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

#include "srsran/adt/span.h"
#include <numeric>

namespace srsran {

/// Calculates the least common multiplier (LCM) for a range of integers.
template <typename Integer>
Integer lcm(span<const Integer> values)
{
  return std::accumulate(
      values.begin(), values.end(), Integer(1), [](Integer a, Integer b) { return std::lcm<Integer>(a, b); });
}

} // namespace srsran
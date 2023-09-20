/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include <numeric>

namespace srsran {

/// Calculates the greatest common divisor (GCD) of two integers.
template <typename Integer>
Integer gcd(Integer a, Integer b)
{
  while (true) {
    if (a == 0) {
      return b;
    }
    b %= a;
    if (b == 0) {
      return a;
    }
    a %= b;
  }
}

/// Calculates the least common multiplier (LCM) of two integers.
template <typename Integer>
Integer lcm(Integer a, Integer b)
{
  Integer temp = gcd(a, b);

  return temp != 0 ? (a / temp * b) : 0;
}

/// Calculates the least common multiplier (LCM) for a range of integers.
template <typename Integer>
Integer lcm(span<const Integer> values)
{
  return std::accumulate(
      values.begin(), values.end(), Integer(1), [](Integer a, Integer b) { return lcm<Integer>(a, b); });
}

} // namespace srsran
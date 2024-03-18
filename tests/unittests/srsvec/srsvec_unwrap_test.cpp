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

/// \file
/// \brief Unwrap test.
///
/// The test takes an array of complex numbers and checks that the output of the \c unwrap_arguments function returns
/// the unwrapped phase angles.

#include "srsran/adt/complex.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/unwrap.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsvec;

static constexpr float tolerance = 1.0e-6;

namespace srsran {

bool operator==(span<const float> lhs, span<const float> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](float lhs_val, float rhs_val) {
    return (std::abs(lhs_val - rhs_val) < tolerance);
  });
}

std::ostream& operator<<(std::ostream& os, span<const float> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsran

TEST(UnwrapTest, RampUp)
{
  constexpr unsigned        length = 10;
  std::array<float, length> phases = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  srsvec::sc_prod(phases, M_PI / 4, phases);

  std::array<cf_t, length> c_numbers;

  std::transform(phases.cbegin(), phases.cend(), c_numbers.begin(), [](float a) { return std::polar(1.0F, a); });

  std::array<float, length> out;

  unwrap_arguments(out, c_numbers);

  ASSERT_EQ(span<const float>(phases), span<const float>(out));
}

TEST(UnwrapTest, RampDown)
{
  constexpr unsigned        length = 10;
  std::array<float, length> phases = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  srsvec::sc_prod(phases, -M_PI / 4, phases);

  std::array<cf_t, length> c_numbers;

  std::transform(phases.cbegin(), phases.cend(), c_numbers.begin(), [](float a) { return std::polar(1.0F, a); });

  std::array<float, length> out;

  unwrap_arguments(out, c_numbers);

  ASSERT_EQ(span<const float>(phases), span<const float>(out));
}

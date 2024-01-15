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
/// \brief Interpolator unit test.

#include "srsran/phy/support/support_factories.h"

#include <gtest/gtest.h>

using namespace srsran;

TEST(InterpolatorTest, FloatAlternate)
{
  std::unique_ptr<interpolator> interp = create_interpolator();

  std::array<cf_t, 5>  input    = {0.0F, 1.0F, 2.0F, 3.0F, 4.0F};
  std::array<cf_t, 10> output   = {};
  std::array<cf_t, 10> expected = {0.0F, 0.5F, 1.0F, 1.5F, 2.0F, 2.5F, 3.0F, 3.5F, 4.0F, 4.0F};

  interp->interpolate(output, input, {0, 2});

  ASSERT_TRUE(std::equal(output.cbegin(), output.cend(), expected.cbegin(), [](cf_t a, cf_t b) {
    return (std::abs(a - b) < 1e-5);
  })) << fmt::format("output:   {},\nexpected: {}", span<cf_t>(output), span<cf_t>(expected));
}

TEST(InterpolatorTest, FloatLateStart)
{
  std::unique_ptr<interpolator> interp = create_interpolator();

  std::array<cf_t, 4>  input    = {1.0F, 1.0F, 2.0F, 4.0F};
  std::array<cf_t, 14> output   = {};
  std::array<cf_t, 14> expected = {
      1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 4.0F / 3, 5.0F / 3, 2.0F, 8.0F / 3, 10.0F / 3, 4.0F, 4.0F, 4.0F};

  interp->interpolate(output, input, {2, 3});

  ASSERT_TRUE(std::equal(output.cbegin(), output.cend(), expected.cbegin(), [](cf_t a, cf_t b) {
    return (std::abs(a - b) < 1e-5);
  })) << fmt::format("output:   {},\nexpected: {}", span<cf_t>(output), span<cf_t>(expected));
}
/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interpolator unit test.

#include "srsgnb/phy/support/support_factories.h"

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
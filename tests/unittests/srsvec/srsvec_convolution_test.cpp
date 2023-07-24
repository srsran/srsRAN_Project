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

/// \file
/// \brief Convolution tests.

#include "srsran/adt/span.h"
#include "srsran/srsvec/convolution.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsvec;

TEST(ConvolutionTest, FloatsOddY)
{
  const std::array<float, 7> x = {1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684};
  const std::array<float, 5> y = {1.2207623, 0.6914214, -0.0194962, -2.0369570, 0.3930559};

  const std::array<float, 7> expected = {
      2.3520038, -0.2047649, -0.8990877, -1.8062155, -3.3081019, -1.3912276, 1.0658817};
  std::array<float, 7> out = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(this_ex, *this_out++, 1e-6);
  }
}

TEST(ConvolutionTest, FloatsEvenY)
{
  const std::array<float, 7> x = {1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684};
  const std::array<float, 4> y = {1.2207623, 0.6914214, -0.0194962, -2.0369570};

  const std::array<float, 7> expected = {
      2.3520038, -0.2047649, -1.3647147, -2.3721664, -3.7522805, -1.8597034, 0.8191883};
  std::array<float, 7> out = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(this_ex, *this_out++, 1e-6);
  }
}

TEST(ConvolutionTest, ComplexY)
{
  const std::array<float, 7> x = {1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684};
  const std::array<cf_t, 2>  y = {cf_t(1.7582890, -0.4543095), cf_t(0.1958175, -0.4767015)};

  const std::array<cf_t, 7> expected = {cf_t(2.7636862, -1.2188648),
                                        cf_t(2.2689323, -1.1997890),
                                        cf_t(2.3169575, -1.0801864),
                                        cf_t(1.3369451, -0.8533095),
                                        cf_t(-0.5698832, -0.1201893),
                                        cf_t(-1.5748091, 0.5747918),
                                        cf_t(-0.1667912, 0.4060393)};
  std::array<cf_t, 7>       out      = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

TEST(ConvolutionTest, ComplexX)
{
  const std::array<cf_t, 6>  x = {cf_t(-0.6606603, -0.6268986),
                                  cf_t(-0.0200462, +0.4951205),
                                  cf_t(-0.2412076, +0.0159381),
                                  cf_t(-0.5587571, -0.4147694),
                                  cf_t(-0.4281389, -0.8600692),
                                  cf_t(-0.0699734, +1.9703149)};
  const std::array<float, 6> y = {0.4453228, -0.6819388, -0.1209226, 0.6317405, -0.7090774, 0.6404318};

  const std::array<cf_t, 6> expected = {cf_t(-0.4992802, -0.6514835),
                                        cf_t(0.6753409, +0.6552190),
                                        cf_t(-0.2329041, +0.7715998),
                                        cf_t(-0.0953035, -1.1958694),
                                        cf_t(-0.0202863, -0.4772853),
                                        cf_t(-0.0984673, +1.5889518)};
  std::array<cf_t, 6>       out      = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

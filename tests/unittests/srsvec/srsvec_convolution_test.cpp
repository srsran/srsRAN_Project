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
/// \brief Convolution tests.

#include "srsran/adt/span.h"
#include "srsran/srsvec/convolution.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsvec;

TEST(ConvolutionTest, FloatsOddY)
{
  const std::array<float, 9> x = {
      1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684, 0.0, 0.0};
  const std::array<float, 5> y = {1.2207623, 0.6914214, -0.0194962, -2.0369570, 0.3930559};

  const std::array<float, 9> expected = {
      2.3520038, -0.2047649, -0.8990877, -1.8062155, -3.3081019, -1.3912276, 1.0658817, 1.5801475, -0.3347926};
  std::array<float, 9> out;

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(this_ex, *this_out++, 1e-6);
  }
}

TEST(ConvolutionTest, FloatsEvenY)
{
  const std::array<float, 9> x = {
      1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684, 0.4059158, 0.7871897};
  const std::array<float, 4> y = {1.2207623, 0.6914214, -0.0194962, -2.0369570};

  const std::array<float, 9> expected = {
      2.3520038, -0.2047649, -1.3647147, -2.3721664, -3.7522805, -1.3641768, 2.0608187, 2.2713816, -0.8421802};
  std::array<float, 9> out;

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(this_ex, *this_out++, 1e-6);
  }
}

TEST(ConvolutionTest, ComplexY)
{
  const std::array<float, 9> x = {
      1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684, 0.6851269, 0.0801917};
  const std::array<cf_t, 2> y = {cf_t(1.7582890, -0.4543095), cf_t(0.1958175, -0.4767015)};

  const std::array<cf_t, 9> expected = {cf_t(+2.7636862, -1.2188648),
                                        cf_t(+2.2689323, -1.1997890),
                                        cf_t(+2.3169575, -1.0801864),
                                        cf_t(+1.3369451, -0.8533095),
                                        cf_t(-0.5698832, -0.1201893),
                                        cf_t(-1.5748091, +0.5747918),
                                        cf_t(+1.0378599, +0.0947796),
                                        cf_t(+0.2751600, -0.3630329),
                                        cf_t(+0.0157029, -0.0382275)};
  std::array<cf_t, 9>       out;

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}
TEST(ConvolutionTest, ComplexEvenX)
{
  const std::array<cf_t, 10> x = {cf_t(-0.6606603, -0.6268986),
                                  cf_t(-0.0200462, +0.4951205),
                                  cf_t(-0.2412076, +0.0159381),
                                  cf_t(-0.5587571, -0.4147694),
                                  cf_t(-0.4281389, -0.8600692),
                                  cf_t(-0.0699734, +1.9703149),
                                  cf_t(+0.5488135, +0.5448832),
                                  cf_t(+0.7151894, +0.4236548),
                                  cf_t(+0.6027634, +0.6458941),
                                  cf_t(+0.09762701, +0.4303787)};
  const std::array<float, 6> y = {0.4453228, -0.6819388, -0.1209226, 0.6317405, -0.7090774, 0.6404318};

  const std::array<cf_t, 10> expected = {cf_t(-0.49928028, -0.65148357),
                                         cf_t(+0.67534087, +0.65521895),
                                         cf_t(-0.23290403, +0.77159989),
                                         cf_t(+0.14909555, -0.95322052),
                                         cf_t(-0.07605338, -0.66019913),
                                         cf_t(-0.3841223, +1.52178787),
                                         cf_t(-0.33192427, -1.90372887),
                                         cf_t(-0.12161369, +0.7715327),
                                         cf_t(+0.21333772, +0.40455143),
                                         cf_t(+0.09229907, +0.08522077)};
  std::array<cf_t, 10>       out;

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

TEST(ConvolutionTest, ComplexOddX)
{
  const std::array<cf_t, 20> x = {
      cf_t(-0.6606603, -0.6268986),   cf_t(-0.0200462, +0.4951205),   cf_t(-0.2412076, +0.0159381),
      cf_t(-0.5587571, -0.4147694),   cf_t(-0.4281389, -0.8600692),   cf_t(-0.0699734, +1.9703149),
      cf_t(+0.5488135, +0.5448832),   cf_t(+0.7151894, +0.4236548),   cf_t(+0.6027634, +0.6458941),
      cf_t(+0.5488135, +0.79172504),  cf_t(+0.71518937, +0.52889492), cf_t(+0.60276338, +0.56804456),
      cf_t(0.54488318, +0.92559664),  cf_t(+0.4236548, +0.07103606),  cf_t(+0.64589411, +0.0871293),
      cf_t(+0.43758721, +0.0202184),  cf_t(+0.891773, +0.83261985),   cf_t(+0.96366276, +0.77815675),
      cf_t(+0.38344152, +0.87001215), cf_t(+0.97861834, +0.79915856)};
  const std::array<float, 7> y = {0.4453228, -0.6819388, -0.1209226, 0.6317405, -0.7090774, 0.6404318, -0.14630237};

  const std::array<cf_t, 20> expected = {
      cf_t(-0.49928028, -0.65148357), cf_t(+0.67534087, +0.65521895), cf_t(-0.23290403, +0.77159989),
      cf_t(+0.24575172, -0.86150377), cf_t(-0.07312057, -0.73263643), cf_t(-0.34883306, +1.51945609),
      cf_t(-0.04925315, -1.68213138), cf_t(-0.0481674, +0.88647576),  cf_t(-0.05027476, -0.03511601),
      cf_t(+0.04215774, +0.19464248), cf_t(+0.08825623, -0.54385628), cf_t(+0.06979336, +0.27481224),
      cf_t(-0.00226595, +0.3458362),  cf_t(+0.18328644, +0.02143291), cf_t(+0.13650044, +0.2906386),
      cf_t(-0.58418233, -0.38283345), cf_t(+0.66254415, +0.22556265), cf_t(-0.55152692, -0.74877886),
      cf_t(-0.05231328, +0.43149044), cf_t(+0.8330349, +0.26449695)};
  std::array<cf_t, 20> out;

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

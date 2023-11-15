/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  std::array<float, 9> out = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(this_ex, *this_out++, 1e-6);
  }
}

TEST(ConvolutionTest, FloatsEvenY)
{
  const std::array<float, 9> x = {
      1.1846331, 1.4398738, 1.1300645, 1.1918809, 0.6276295, -0.3940103, -0.8517684, 0.0, 0.0};
  const std::array<float, 4> y = {1.2207623, 0.6914214, -0.0194962, -2.0369570};

  const std::array<float, 9> expected = {
      2.3520038, -0.2047649, -1.3647147, -2.3721664, -3.7522805, -1.8597034, 0.8191883, 1.7350156, 0.0};
  std::array<float, 9> out = {};

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

  const std::array<cf_t, 9> expected = {cf_t(2.7636862, -1.2188648),
                                        cf_t(2.2689323, -1.1997890),
                                        cf_t(2.3169575, -1.0801864),
                                        cf_t(1.3369451, -0.8533095),
                                        cf_t(-0.5698832, -0.1201893),
                                        cf_t(-1.5748091, 0.5747918),
                                        cf_t(1.0378599, 0.0947796),
                                        cf_t(0.2751600, -0.3630329),
                                        cf_t(0.0157029, -0.0382275)};
  std::array<cf_t, 9>       out      = {};

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
                                  cf_t(0.5488135, +0.5448832),
                                  cf_t(0.7151894, +0.4236548),
                                  cf_t(0.6027634, +0.6458941),
                                  cf_t(0.0, 0.0)};
  const std::array<float, 6> y = {0.4453228, -0.6819388, -0.1209226, 0.6317405, -0.7090774, 0.6404318};

  const std::array<cf_t, 10> expected = {cf_t(-0.49928028, -0.65148357),
                                         cf_t(0.67534087, +0.65521895),
                                         cf_t(-0.23290403, +0.77159989),
                                         cf_t(0.14909555, -0.95322052),
                                         cf_t(-0.07605338, -0.66019913),
                                         cf_t(-0.3841223, +1.52178787),
                                         cf_t(-0.37539981, -2.09538633),
                                         cf_t(-0.05503805, +1.06502466),
                                         cf_t(0.22514303, +0.45659395),
                                         cf_t(0.03062413, -0.1866669)};
  std::array<cf_t, 10>       out      = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

TEST(ConvolutionTest, ComplexOddX)
{
  const std::array<cf_t, 20> x = {
      cf_t(-0.6606603, -0.6268986),  cf_t(-0.0200462, +0.4951205),  cf_t(-0.2412076, +0.0159381),
      cf_t(-0.5587571, -0.4147694),  cf_t(-0.4281389, -0.8600692),  cf_t(-0.0699734, +1.9703149),
      cf_t(0.5488135, +0.5448832),   cf_t(0.7151894, +0.4236548),   cf_t(0.6027634, +0.6458941),
      cf_t(0.5488135, +0.79172504),  cf_t(0.71518937, +0.52889492), cf_t(0.60276338, +0.56804456),
      cf_t(0.54488318, +0.92559664), cf_t(0.4236548, +0.07103606),  cf_t(0.64589411, +0.0871293),
      cf_t(0.43758721, +0.0202184),  cf_t(0.891773, +0.83261985),   cf_t(0.96366276, +0.77815675),
      cf_t(0.38344152, +0.87001215), cf_t(0.97861834, +0.79915856)};
  const std::array<float, 7> y = {0.4453228, -0.6819388, -0.1209226, 0.6317405, -0.7090774, 0.6404318, 0.0};

  const std::array<cf_t, 20> expected = {
      cf_t(-0.49928028, -0.65148357), cf_t(0.67534087, +0.65521895),  cf_t(-0.23290403, +0.77159989),
      cf_t(0.14909555, -0.95322052),  cf_t(-0.07605338, -0.66019913), cf_t(-0.3841223, +1.52178787),
      cf_t(-0.13100064, -1.74281312), cf_t(-0.11080514, +0.7606456),  cf_t(-0.06051203, +0.25314572),
      cf_t(0.12245046, +0.27436018),  cf_t(0.19289013, -0.48187458),  cf_t(0.15797908, +0.36930808),
      cf_t(0.07802677, +0.46166745),  cf_t(0.28792033, +0.09881149),  cf_t(0.22468615, +0.37374487),
      cf_t(-0.50446463, -0.24741648), cf_t(0.72452585, +0.23595539),  cf_t(-0.45703108, -0.73603164),
      cf_t(0.01170677, +0.43444844),  cf_t(0.9635034, +0.38631121)};
  std::array<cf_t, 20> out = {};

  convolution_same(out, x, y);

  const auto* this_out = out.cbegin();
  for (auto this_ex : expected) {
    ASSERT_NEAR(std::abs(this_ex - *this_out++), 0.0F, 1e-6);
  }
}

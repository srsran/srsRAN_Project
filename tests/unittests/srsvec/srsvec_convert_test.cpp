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

#include "srsran/srsvec/conversion.h"
#include "srsran/support/srsran_test.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-5;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-5;
/// Note: SIMD and non-SIMD float-to-integer conversions may produce different results.
/// For instance, Intel’s intrinsics round to the nearest integer with ties to even (2.5 -> 2), while std::round rounds
/// halfway cases away from zero (2.5 -> 3). A +/-1 tolerance is applied to account for these differing rounding
/// behaviors.
static const float ASSERT_ROUNDING_MAX_ERROR = 1;

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const cbf16_t& value)
{
  fmt::print(os, "{}", value);
  return os;
}

} // namespace srsran

namespace {

using SrsvecConvertParams = unsigned;

class SrsvecConvertFixture : public ::testing::TestWithParam<SrsvecConvertParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    // Get test parameters.
    auto params = GetParam();
    size        = params;
  }
};

using namespace srsran;

TEST_P(SrsvecConvertFixture, SrsvecConvertTestComplexInt16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  std::vector<int16_t> z(2 * size);

  float scale = 1000.0F;

  srsvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    int16_t gold_re = static_cast<int16_t>(std::round(x[i].real() * scale));
    int16_t gold_im = static_cast<int16_t>(std::round(x[i].imag() * scale));
    ASSERT_NEAR(gold_re, z[2 * i + 0], ASSERT_ROUNDING_MAX_ERROR);
    ASSERT_NEAR(gold_im, z[2 * i + 1], ASSERT_ROUNDING_MAX_ERROR);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestInt16Complex)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x(2 * size);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  std::vector<cf_t> z(size);

  float scale = 1000.0F;

  srsvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold = {static_cast<float>(x[2 * i]) / scale, static_cast<float>(x[2 * i + 1]) / scale};
    float err  = std::abs(gold - z[i]);
    TESTASSERT(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestFloatInt16)
{
  std::uniform_real_distribution<float> dist(-1, 1);

  std::vector<float> x(size);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<int16_t> z(size);

  float scale = 1000.0F;

  srsvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    int16_t gold = static_cast<int16_t>(std::round(x[i] * scale));
    ASSERT_NEAR(gold, z[i], ASSERT_ROUNDING_MAX_ERROR);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestInt16Float)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x(size);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  std::vector<float> z(size);

  float scale = 1000.0F;

  srsvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    float gold = static_cast<float>(x[i]) / scale;
    float err  = std::abs(gold - z[i]);
    TESTASSERT(err < ASSERT_FLOAT_MAX_ERROR);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestComplexComplex16Random)
{
  static constexpr float                range = std::numeric_limits<float>::max() / 2;
  std::uniform_real_distribution<float> dist(-range, range);

  std::vector<cf_t> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<cf_t>    out(size);
  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precission to BF16.
  srsvec::convert(data_cbf16, in);

  // Convert from BF16 to single precision.
  srsvec::convert(out, data_cbf16);

  // Assert converstion from single precision.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_cbf16[i], to_cbf16(in[i]));
  }

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestComplexComplex16Special)
{
  static const float nan            = std::numeric_limits<float>::quiet_NaN();
  static const float infinity       = std::numeric_limits<float>::infinity();
  static const float neg_infinity   = -infinity;
  static const float one_round_down = 1.0F + std::pow(2.0F, -8.0F);
  static const float one_round_up   = one_round_down + std::pow(2.0F, -7.0F);

  static const std::vector<float>    values          = {nan, infinity, neg_infinity, one_round_down, one_round_up};
  static const std::vector<uint16_t> expected_values = {0x7fc0, 0x7f80, 0xff80, 0x3f80, 0x3f82};

  std::vector<cf_t> in(size);
  std::generate(in.begin(), in.end(), [n = 0]() mutable {
    float re = values[(n++) % values.size()];
    float im = values[(n++) % values.size()];
    return cf_t(re, im);
  });

  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precission to BF16.
  srsvec::convert(data_cbf16, in);

  // Verify the converted values are equal to the expected.
  for (unsigned i = 0; i != size; ++i) {
    cbf16_t expected;
    expected.real = bf16_t(expected_values[(2 * i) % expected_values.size()]);
    expected.imag = bf16_t(expected_values[(2 * i + 1) % expected_values.size()]);
    ASSERT_EQ(expected, data_cbf16[i]);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestFloatFloat16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return dist(rgen); });

  // Convert from single precision to brain float.
  std::vector<bf16_t> data_bf16(size);
  srsvec::convert(data_bf16, in);

  // Assert conversion to BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_bf16[i], to_bf16(in[i]));
  }

  // Convert back to single precision float.
  std::vector<float> out(size);
  srsvec::convert(out, data_bf16);

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestInt16Float16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  float int16_scale = (1 << 15) - 1;

  std::vector<float> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return dist(rgen); });

  // Convert from single precision to int16.
  std::vector<int16_t> in_int16(size);
  srsvec::convert(in_int16, in, int16_scale);

  // Convert from int16 to brain float.
  std::vector<bf16_t> data_bf16(size);
  srsvec::convert(data_bf16, in_int16, int16_scale);

  // Assert conversion to BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_bf16[i], to_bf16(in_int16[i], int16_scale));
  }

  // Convert from brain float back to int16.
  std::vector<int16_t> out_int16(size);
  srsvec::convert(out_int16, data_bf16, int16_scale);

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(out_int16[i], to_int16(data_bf16[i], int16_scale));
  }

  // Convert int16 to float and compare with original data.
  std::vector<float> out(size);
  srsvec::convert(out, out_int16, int16_scale);

  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F + 1 / int16_scale;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(SrsvecConvertFixture, SrsvecConvertTestScaledInt16ComplexFloat16Random)
{
  constexpr float int16_gain = 1.0 / ((1 << 15) - 1);

  std::uniform_int_distribution<int16_t> dist_i(-32768, 32767);
  std::uniform_int_distribution<int16_t> dist_f(1, 128);

  const unsigned size_i16 = size * 2;

  std::vector<int16_t> in(size_i16);
  std::vector<float>   gain(size_i16);

  std::generate(in.begin(), in.end(), [&dist_i]() { return dist_i(rgen); });
  std::generate(gain.begin(), gain.end(), [&dist_f]() { return int16_gain * float(dist_f(rgen)); });

  // Convert from int16 to brain float.
  std::vector<cbf16_t> data_cbf16(size);
  srsvec::convert(data_cbf16, in, gain);

  // Assert conversion to cbf16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_cbf16[i].real, to_bf16(in[i * 2], 1 / gain[i * 2]));
    ASSERT_EQ(data_cbf16[i].imag, to_bf16(in[i * 2 + 1], 1 / gain[i * 2 + 1]));
  }
}

INSTANTIATE_TEST_SUITE_P(SrsvecConvertTest, SrsvecConvertFixture, ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace

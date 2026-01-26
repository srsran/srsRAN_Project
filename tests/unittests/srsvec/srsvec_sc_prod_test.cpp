/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

namespace {
using SrsvecScProdFixtureParams = unsigned;

class SrsvecScProdFixture : public ::testing::TestWithParam<SrsvecScProdFixtureParams>
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

TEST_P(SrsvecScProdFixture, SrsvecScProdFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  cf_t h = {dist(rgen), dist(rgen)};

  std::vector<cf_t> z(size);

  srsvec::sc_prod(z, x, h);

  for (size_t i = 0; i != size; i++) {
    cf_t  gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

TEST_P(SrsvecScProdFixture, SrsvecScProdBrainFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(size);
  std::generate(x.begin(), x.end(), [&dist]() { return to_cbf16(cf_t{dist(rgen), dist(rgen)}); });

  cf_t h = {dist(rgen), dist(rgen)};

  std::vector<cbf16_t> z(size);

  srsvec::sc_prod(z, x, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [&h](cbf16_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    cf_t  gold_z = to_cf(x[i]) * h;
    float err    = std::abs(gold_z - to_cf(z[i]));
    TESTASSERT(err < 5e-3, " err={}", err);
  }
}

TEST_P(SrsvecScProdFixture, SrsvecScProdBrainFloatComplexReal)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(size);
  std::generate(x.begin(), x.end(), [&dist]() { return to_cbf16(cf_t{dist(rgen), dist(rgen)}); });

  float h = dist(rgen);

  std::vector<cbf16_t> z(size);

  srsvec::sc_prod(z, x, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [&h](cbf16_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    cf_t  gold_z = to_cf(x[i]) * h;
    float err    = std::abs(gold_z - to_cf(z[i]));
    TESTASSERT(err < 5e-3, " err={}", err);
  }
}

TEST_P(SrsvecScProdFixture, SrsvecScProdFloatComplexReal)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float h = dist(rgen);

  std::vector<cf_t> z(size);

  srsvec::sc_prod(z, x, h);

  for (size_t i = 0; i != size; i++) {
    cf_t  gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

TEST_P(SrsvecScProdFixture, SrsvecScProdFloat)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(size);
  for (float& v : x) {
    v = dist(rgen);
  }

  float h = dist(rgen);

  std::vector<float> z(size);

  srsvec::sc_prod(z, x, h);

  for (size_t i = 0; i != size; i++) {
    float gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

INSTANTIATE_TEST_SUITE_P(SrsvecScProdTest, SrsvecScProdFixture, ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace

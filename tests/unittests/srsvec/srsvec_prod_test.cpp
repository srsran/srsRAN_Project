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

#include "srsran/srsvec/prod.h"
#include "srsran/support/math/math_utils.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR      = 1e-6;
static const float  ASSERT_MAX_ERROR_CEXP = 1e-5;

using namespace srsran;

class SrsVecProdFixture : public ::testing::TestWithParam<unsigned>
{
protected:
  unsigned N = 0;

  void SetUp() override { N = GetParam(); }
};

TEST_P(SrsVecProdFixture, ProdCCC)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> y(N);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> z(N);

  srsvec::prod(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
  }
}

TEST_P(SrsVecProdFixture, ProdFFF)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<float> y(N);
  for (float& v : y) {
    v = dist(rgen);
  }

  std::vector<float> z(N);

  srsvec::prod(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
  }
}

TEST_P(SrsVecProdFixture, ProdCexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cf_t> z(N);

  srsvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);
  for (size_t i = 0; i != N; ++i) {
    cf_t gold_z = x[i] * phase;
    phase *= osc;
    float err = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR_CEXP) << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(srsvec, SrsVecProdFixture, ::testing::Values(1, 5, 7, 19, 23, 123, 257));

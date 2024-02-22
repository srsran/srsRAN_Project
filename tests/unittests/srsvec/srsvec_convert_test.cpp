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

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-3;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-3;

using namespace srsran;

void test_convert_ci(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<int16_t> z(2 * N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    int16_t gold_re = static_cast<int16_t>(std::round(x[i].real() * scale));
    int16_t gold_im = static_cast<int16_t>(std::round(x[i].imag() * scale));
    TESTASSERT_EQ(gold_re, z[2 * i + 0]);
    TESTASSERT_EQ(gold_im, z[2 * i + 1]);
  }
}

void test_convert_ic(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  srsvec::aligned_vec<int16_t> x(2 * N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<cf_t> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold = {static_cast<float>(x[2 * i]) / scale, static_cast<float>(x[2 * i + 1]) / scale};
    float err  = std::abs(gold - z[i]);
    TESTASSERT(err < ASSERT_CF_MAX_ERROR);
  }
}

void test_convert_fi(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1, 1);

  srsvec::aligned_vec<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    int16_t gold = static_cast<int16_t>(std::round(x[i] * scale));
    TESTASSERT(gold == z[i]);
  }
}

void test_convert_if(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  srsvec::aligned_vec<int16_t> x(N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    float gold = static_cast<float>(x[i]) / scale;
    float err  = std::abs(gold - z[i]);
    TESTASSERT(err < ASSERT_FLOAT_MAX_ERROR);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_convert_ci(N);
    test_convert_ic(N);
    test_convert_fi(N);
    test_convert_if(N);
  }
}

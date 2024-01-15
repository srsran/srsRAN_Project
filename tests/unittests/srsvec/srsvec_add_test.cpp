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

#include "srsran/srsvec/add.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-6;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-6;

using namespace srsran;

void test_cf_add(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<cf_t> y(N);
  for (cf_t& v : y) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<cf_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_CF_MAX_ERROR);
  }
}

void test_float_add(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> y(N);
  for (float& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    float gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_FLOAT_MAX_ERROR);
  }
}

void test_i16_add(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN / 2, INT16_MAX / 2);

  srsvec::aligned_vec<int16_t> x(N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> y(N);
  for (int16_t& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    int gold_z = x[i] + y[i];
    int err    = std::abs(gold_z - z[i]);
    TESTASSERT_EQ(err, 0);
  }
}

void test_i8_add(std::size_t N)
{
  std::uniform_int_distribution<int8_t> dist(INT8_MIN / 2, INT8_MAX / 2);

  srsvec::aligned_vec<int8_t> x(N);
  for (int8_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int8_t> y(N);
  for (int8_t& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int8_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    int gold_z = x[i] + y[i];
    int err    = std::abs(gold_z - z[i]);
    TESTASSERT_EQ(err, 0);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_cf_add(N);
    test_float_add(N);
    test_i16_add(N);
    test_i8_add(N);
  }
}
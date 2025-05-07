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

#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-5;

using namespace srsran;

static void test_dot_prod_ccc(std::size_t N)
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

  cf_t z = srsvec::dot_prod(x, y);

  cf_t gold_z = std::inner_product(
      x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cf_t b) { return a * std::conj(b); });

  float err = std::abs(z - gold_z);
  TESTASSERT(err < ASSERT_MAX_ERROR,
             "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
             err,
             ASSERT_MAX_ERROR,
             N,
             gold_z,
             z);
}

static void test_dot_prod_ccbf16(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cbf16_t> y(N);
  for (cbf16_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  cf_t z1 = srsvec::dot_prod(x, y);
  cf_t z2 = srsvec::dot_prod(y, x);

  cf_t gold_z1 = std::inner_product(x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cbf16_t b) {
    return a * std::conj(to_cf(b));
  });
  cf_t gold_z2 = std::conj(z1);

  float err1 = std::abs(z1 - gold_z1);
  TESTASSERT(err1 < ASSERT_MAX_ERROR,
             "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
             err1,
             ASSERT_MAX_ERROR,
             N,
             gold_z1,
             z1);

  float err2 = std::abs(z2 - gold_z2);
  TESTASSERT(err2 < ASSERT_MAX_ERROR,
             "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
             err2,
             ASSERT_MAX_ERROR,
             N,
             gold_z2,
             z2);
}

static void test_avg_power_cf(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float z = srsvec::average_power(x);

  float expected =
      std::accumulate(
          x.begin(), x.end(), 0.0F, [](float acc, cf_t in) { return acc + std::real(in * std::conj(in)); }) /
      static_cast<float>(N);

  float err = std::abs(z - expected);
  TESTASSERT(err < ASSERT_MAX_ERROR,
             "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
             err,
             ASSERT_MAX_ERROR,
             N,
             expected,
             z);
}

static void test_avg_power_cbf16(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(N);
  for (cbf16_t& v : x) {
    v = cbf16_t(dist(rgen), dist(rgen));
  }

  float z = srsvec::average_power(x);

  float expected =
      std::accumulate(x.begin(),
                      x.end(),
                      0.0F,
                      [](float acc, cbf16_t in) { return acc + std::real(to_cf(in) * std::conj(to_cf(in))); }) /
      static_cast<float>(N);

  float err = std::abs(z - expected);
  TESTASSERT(err < ASSERT_MAX_ERROR,
             "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
             err,
             ASSERT_MAX_ERROR,
             N,
             expected,
             z);
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 65, 130, 257};

  for (std::size_t N : sizes) {
    test_dot_prod_ccc(N);
    test_dot_prod_ccbf16(N);
    test_avg_power_cf(N);
    test_avg_power_cbf16(N);
  }
}

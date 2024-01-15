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
#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 3e-6;

using namespace srsran;

static void test_dot_prod_ccc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  srsvec::aligned_vec<cf_t> y(N);
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

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 65, 130, 257};

  for (std::size_t N : sizes) {
    test_dot_prod_ccc(N);
  }
}

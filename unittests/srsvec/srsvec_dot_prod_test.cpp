/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 2e-6;

using namespace srsgnb;

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

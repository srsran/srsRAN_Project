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
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsgnb;

static void test_max_abs_ccc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::pair<unsigned, float> result = srsvec::max_abs_element(x);

  cf_t*    expected_it = std::max_element(x.begin(), x.end(), [](cf_t a, cf_t b) { return abs_sq(a) < abs_sq(b); });
  unsigned expected_max_index = static_cast<unsigned>(expected_it - x.begin());
  float    expected_max_value = abs_sq(*expected_it);

  TESTASSERT_EQ(expected_max_index, result.first);
  TESTASSERT_EQ(expected_max_value, result.second);
}

static void test_max_abs_ccc_same(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  std::fill(x.begin(), x.end(), 0);

  std::pair<unsigned, float> result = srsvec::max_abs_element(x);

  TESTASSERT_EQ(0, result.first);
  TESTASSERT_EQ(0.0, result.second);
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 65, 130, 257};

  for (std::size_t N : sizes) {
    test_max_abs_ccc(N);
    test_max_abs_ccc_same(N);
  }
}

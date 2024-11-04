/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/modulus_square.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

void test_modulus_square(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> input(N);
  for (cf_t& v : input) {
    v = {dist(rgen), dist(rgen)};
  }

  srsvec::aligned_vec<float> result(N);

  srsvec::modulus_square(result, input);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = abs_sq(input[i]);
    float err    = std::abs(gold_z - result[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_modulus_square(N);
  }
}
